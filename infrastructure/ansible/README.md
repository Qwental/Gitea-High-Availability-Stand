
## deploy_db.yml

Плейбук для развертывания Master-Slave HA кластера PostgreSQL.

## Назначение
Автоматизация полного цикла подготовки БД: от установки пакетов до настройки миграции VIP-адреса.

## Host Groups
- `db_cluster`: Список всех узлов базы данных (db-01, db-02).

## Использование

```bash
ansible-playbook -i inventory.ini deploy_db.yml
```

## Порядок выполнения (Workflow)
1. **postgresql**:
   - Настройка `postgresql.conf` (max_wal_senders, wal_level=replica).
   - Применение systemd-патчей для Docker.
   - Инициализация репликации на Slave-узлах.
2. **keepalived**:
   - Настройка VRRP-инстанса.
   - Запуск мониторинга локального Postgres.

## Tags
- `install`: Только установка пакетов.
- `config`: Только обновление конфигурации без перезагрузки (если возможно).
- `service`: Управление состоянием служб.

## Author
Vladimir Bugrenkov