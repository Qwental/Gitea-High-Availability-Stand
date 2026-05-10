# Описание роли: postgresql

Роль предназначена для развертывания отказоустойчивого кластера PostgreSQL в режиме Master-Replica с автоматическим failover под управлением `repmgr` и `repmgrd`. Включает настройку базы данных для приложения и экспорт метрик.

## Структура задач (Tasks)

Роль разделена на три логических плейбука (файла задач):

### 1. `main.yml` (Core & HA Configuration)

Основной процесс инициализации кластера и настройки репликации.

* **Установка пакетов**: PostgreSQL (предположительно v14/15), `repmgr`.
* **Инициализация (Bootstrapping)**:
* Поведение ветвится на основе переменной `is_master | bool`.
* **Master**: Инициализирует первичный кластер, настраивает `postgresql.conf`, `pg_hba.conf`, `repmgr.conf` и регистрирует primary-ноду (`repmgr primary register`).
* **Standby (Replica)**: Ожидает доступности мастера, клонирует данные через `repmgr standby clone` и регистрирует себя как реплику.


* **Workaround для Docker-контейнеров**: Реализовано явное создание директории `/etc/postgresql/<version>/main`. *Причина*: при пересоздании контейнера эфемерная директория `/etc` очищается, но Docker Volume с `/var/lib/postgresql` сохраняется, из-за чего стандартный скрипт `pg_createcluster` (Debian/Ubuntu) пропускает генерацию конфигов.
* **Запуск `repmgrd**`: Включение демона для автоматического мониторинга и failover.

### 2. `gitea.yml` (Application DB Setup)

Идемпотентная настройка целевой базы данных.

* Выполняется **только на мастере** (`when: is_master | bool`).
* Выполняет создание базы данных (`gitea`) и пользователя (`gitea`) с нужными правами.
* *Зависимость*: Требует корректной настройки `pg_hba.conf` для подключения модуля `postgresql_db`/`postgresql_user` (локально через сокет или TCP).

### 3. `postgres_exporter.yml` (Observability)

Интеграция с подсистемой мониторинга Prometheus.

* Загрузка и установка бинарного файла `postgres_exporter`.
* Настройка конфигурации аутентификации (шаблон `pgpass.j2` для пользователя мониторинга).
* Создание и запуск systemd-юнита `postgres_exporter.service`.

## Переменные роли (Role Variables)

### Логика кластера (host_vars/inventory)

| Переменная | Описание |
| --- | --- |
| `is_master` | `true` для `db-node-01`, `false` для `db-node-02`. Определяет флоу инициализации (Master/Standby). |

### База данных и доступы (group_vars/vault)

| Переменная | Описание |
| --- | --- |
| `pg_admin_password` | Пароль суперпользователя базы данных (Vault). |
| `pg_repl_password` | Пароль пользователя `repmgr` для стриминговой репликации (Vault). |
| `gitea_db_name` | Имя базы данных для Gitea. |
| `gitea_db_user` | Имя пользователя для Gitea. |
| `gitea_db_password` | Пароль пользователя Gitea (Vault). |

## Архитектурные решения и особенности (Architecture Notes)

1. **Replication Topology**: Асинхронная потоковая репликация (streaming replication). Управляется `repmgr`.
2. **Failover (repmgrd)**: Демон непрерывно пингует мастера. При недоступности (параметры `reconnect_attempts`, `reconnect_interval`) реплика автоматически повышается до мастера (`repmgr standby promote`).
3. **Implicit Dependencies**:
* Для корректной работы `gitea.yml` требуется, чтобы PostgreSQL был полностью запущен, а порт 5432 доступен.
* Роль жестко зависит от роли `keepalived`, которая управляет плавающим VIP-адресом. Переключение VIP при смене мастера должно синхронизироваться (обычно через параметры `event_notification_command` в `repmgr.conf`), чтобы трафик Gitea отправлялся на новый мастер.


4. **Уязвимости (Weak points)**:
* Отсутствие защиты от Split-Brain. Если связь между `db-node-01` и `db-node-02` пропадает, но обе ноды видят сеть, `repmgrd` может повысить реплику до мастера, создав два активных мастера. Требуется настройка `witness` ноды или внешнего механизма ограждения (fencing/STONITH).
* Ошибки `pg_hba.conf`: Диагностика показывает, что доступ по сокету для служебных пользователей требует явной конфигурации (например, замена `peer` на `md5`/`scram-sha-256` или маппинг пользователей ОС).



## Пример использования (Example Playbook)

```yaml
- name: Deploy PostgreSQL Cluster
  hosts: db_cluster
  become: yes
  roles:
    - role: postgresql
      tags: database

```