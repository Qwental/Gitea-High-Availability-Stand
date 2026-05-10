# Описание роли: monitoring_agents

Роль предназначена для развертывания и настройки агентов сбора метрик и логов на всех узлах инфраструктуры. Обеспечивает интеграцию хостов с централизованной системой мониторинга (Prometheus) и логирования (Loki).

## Требования (Requirements)

* **ОС**: Ubuntu/Debian.
* **Зависимости пакетов**: `unzip`, `tar` (для распаковки бинарных файлов).
* **Сетевой доступ**:
* Исходящий к `monitor-node-01:3100` (Loki API).
* Входящий на порт `9100/TCP` (Node Exporter) для сбора метрик сервером Prometheus.



## Переменные роли (Role Variables)

| Переменная | Описание | По умолчанию |
| --- | --- | --- |
| `node_exporter_version` | Версия Prometheus Node Exporter. | `1.7.0` |
| `promtail_version` | Версия Grafana Promtail. | `2.9.3` |
| `target_arch` | Архитектура процессора (`amd64`, `arm64`). | - |

## Техническая реализация (Architecture)

### 1. Сбор метрик (Node Exporter)

* Устанавливается как бинарный файл в `/usr/local/bin/`.
* Запускается под выделенным системным пользователем `monitoring` с минимальными привилегиями.
* Экспортирует аппаратные и системные метрики ОС (CPU, Mem, Disk, Network).

### 2. Сбор логов (Promtail)

* **Механизм**: Promtail настроен на сбор данных из двух источников:
1. **Текстовые логи**: Файлы по пути `/var/log/*log`.
2. **Systemd Journal**: Прямое чтение системного журнала.


* **Конфигурация**: Шаблон `promtail.yaml.j2` динамически определяет адрес сервера Loki, используя `hostvars['monitor-node-01']['ansible_host']`.
* **Привилегии**: Сервис запускается от имени `root`. Это необходимо для обеспечения беспрепятственного доступа к `/var/log/journal` и бинарным логам systemd.

### 3. Настройка Journald (`tasks/journald.yml`)

Роль принудительно переводит `systemd-journald` в режим **persistent** хранения:

* Создает конфигурацию `/etc/systemd/journald.conf.d/persistent.conf` со значением `Storage=persistent`.
* Гарантирует наличие директории `/var/log/journal`.
* Это решение обеспечивает сохранность логов после перезагрузки контейнеров/нод, что критично для диагностики причин падения сервисов.

## Метки логирования (Labels)

Для удобной фильтрации в Grafana/Loki к логам добавляются следующие лейблы:

* `host`: Имя хоста из инвентаря Ansible.
* `job`: Тип лога (`varlogs` или `systemd-journal`).
* `unit`: Имя systemd-юнита (только для journal-логов, извлекается через `relabel_configs`).

## Взаимодействие с другими компонентами

* **Monitoring Server**: Собирает метрики, опрашивая Node Exporter на каждом агенте. Принимает push-потоки логов от Promtail.

## Пример использования (Example Playbook)

```yaml
- name: Deploy Monitoring Agents
  hosts: all
  become: yes
  roles:
    - role: monitoring_agents
      tags: agents

```