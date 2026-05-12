## Описание

Роль предназначена для развертывания центрального узла мониторинга и логирования. Она обеспечивает автоматическую установку, настройку и запуск стека Observability.

**Основные возможности:**

* Развертывание **Prometheus** для сбора метрик.
* Развертывание **Loki** для агрегации логов.
* Развертывание **Alertmanager** для управления уведомлениями.
* Установка и настройка **Grafana** (через официальный репозиторий APT).
* Автоматический **provisioning** источников данных (Prometheus, Loki).
* Автоматический импорт дашбордов из локальных JSON-файлов с коррекцией UID источников данных.
* Настройка системного времени и часового пояса.

## Требования

* **ОС:** Ubuntu 20.04 LTS или выше.
* **Ansible:** версии 2.10 или выше.
* **Права:** наличие `sudo` или работа под пользователем `root`.
* **Зависимости:** на целевом хосте должен быть доступ к интернету для загрузки бинарных файлов с GitHub и пакетов из репозитория Grafana.

## Переменные роли

### Defaults (`defaults/main.yml`)

Эти переменные имеют самый низкий приоритет и предназначены для настройки версий и базовых параметров.

| Переменная | Тип | Значение по умолчанию | Описание |
| --- | --- | --- | --- |
| `prometheus_version` | string | `2.45.0` | Версия Prometheus для загрузки. |
| `alertmanager_version` | string | `0.25.0` | Версия Alertmanager для загрузки. |
| `loki_version` | string | `2.8.2` | Версия Loki для загрузки. |
| `target_arch` | string | `amd64` | Архитектура системы (`amd64`, `arm64`). |
| `grafana_prometheus_uid` | string | `prometheus-internal` | Статический UID для источника данных Prometheus. |
| `grafana_loki_uid` | string | `loki-internal` | Статический UID для источника данных Loki. |

### Vault / Секреты

| Переменная | Тип | Значение по умолчанию | Описание |
| --- | --- | --- | --- |
| `vault_grafana_admin_password` | string | `admin` | Пароль администратора Grafana. |

## Обработчики (Handlers)

Роль использует следующие хендлеры для минимизации перезапусков сервисов:

* `restart monitoring services`: Перезапускает Prometheus, Loki и Alertmanager при изменении их бинарных файлов или конфигураций.
* `restart grafana`: Перезапускает `grafana-server` при изменении параметров безопасности (`grafana.ini`), источников данных или дашбордов.

## Пример использования

```yaml
- name: Deploy Monitoring Infrastructure
  hosts: monitoring_servers
  become: yes
  roles:
    - role: monitoring_server
      vars:
        prometheus_version: "2.47.0"
        target_arch: "amd64"
        vault_grafana_admin_password: "{{ secret_password_from_vault }}"

```

## Структура файлов дашбордов

Файлы дашбордов должны быть размещены в папке `files/dashboards/` внутри роли.
При деплое роль автоматически выполняет:

1. Копирование JSON-файлов в `/var/lib/grafana/dashboards`.
2. Замену плейсхолдеров `${DS_PROMETHEUS}` и `${DS_LOKI}` (регистронезависимо) на соответствующие UID, заданные в переменных

## Проверка работоспособности (Healthchecks)

В конце выполнения роль выполняет проверку доступности API всех сервисов:

* Prometheus: `http://localhost:9090/-/healthy`
* Loki: `http://localhost:3100/ready`
* Alertmanager: `http://localhost:9093/-/healthy`
* Grafana: `http://localhost:3000/api/health`