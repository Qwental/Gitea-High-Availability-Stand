# postgresql

Роль для развертывания и настройки отказоустойчивого узла СУБД PostgreSQL 14 в контейнеризованной среде с поддержкой потоковой репликации

## Requirements

- **Ansible** >= 2.14
- **OS**: Ubuntu 22.04 (Docker Container)
- **Python** >= 3.10
- **Collections**: `community.postgresql`

## Role Variables

| Variable | Default | Description |
| :--- | :--- | :--- |
| `pg_version` | "14" | Версия устанавливаемого пакета PostgreSQL |
| `pg_data_dir` | "/var/lib/postgresql/14/main" | Путь к директории данных СУБД |
| `pg_conf_dir` | "/etc/postgresql/14/main" | Путь к конфигурационным файлам |
| `pg_repl_user` | "replicator" | Имя системного пользователя для репликации |
| `pg_repl_password` | "change_me" | Пароль пользователя репликации (рекомендуется Vault) |
| `pg_master_ip` | "172.20.0.11" | IP-адрес мастера для инициализации репликации |
| `is_master` | false | Флаг роли узла (true для Master, false для Slave) |

## Dependencies

None

## Example Playbook

```yaml
- hosts: db_cluster
  roles:
    - role: postgresql
      vars:
        is_master: "{{ inventory_hostname == 'db-01' }}"
        pg_repl_password: "{{ vault_pg_repl_password }}"
```

## Notes
- Роль идемпотентна.
- Реализован `systemd override` (Type=simple) для работы в Docker.
- Автоматически выполняет `pg_basebackup` на узлах, где `is_master: false`.
