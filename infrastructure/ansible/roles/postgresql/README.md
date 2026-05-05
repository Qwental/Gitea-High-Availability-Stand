# postgresql

Роль для развертывания и настройки отказоустойчивого узла СУБД PostgreSQL 14 в контейнеризованной среде с поддержкой потоковой репликации и автоматическим созданием БД для Gitea.

## Requirements

- **Ansible** >= 2.14
- **OS**: Ubuntu 22.04 (Docker Container)
- **Python** >= 3.10
- **Collections**: `community.postgresql` (необходим для управления БД, пользователями и правами)

## Role Variables

К базовым настройкам репликации добавлены переменные для инициализации базы данных Gitea:

| Variable | Default | Description |
| :--- | :--- | :--- |
| `pg_version` | "14" | Версия устанавливаемого пакета PostgreSQL |
| `pg_data_dir` | "/var/lib/postgresql/14/main" | Путь к директории данных СУБД |
| `pg_conf_dir` | "/etc/postgresql/14/main" | Путь к конфигурационным файлам |
| `pg_repl_user` | "replicator" | Имя системного пользователя для репликации |
| `pg_repl_password` | "change_me" | Пароль пользователя репликации |
| `pg_master_ip` | "172.20.0.11" | IP-адрес мастера для инициализации репликации |
| `is_master` | false | Флаг роли узла (true для Master, false для Slave) |
| **Gitea Integration** | | |
| `gitea_db_user` | "gitea" | Имя пользователя базы данных Gitea |
| `gitea_db_password` | `""` | Пароль пользователя (обязательно через Vault) |
| `gitea_db_name` | "gitea" | Название целевой базы данных |

## Tasks Description

Роль разделена на логические блоки:
1. **`main.yml`**: Основной цикл установки PG, настройки `postgresql.conf`, `pg_hba.conf` и управления состоянием службы. Здесь же реализована логика `pg_basebackup` для реплик.
2. **`gitea.yml`**: Создание пользователя, базы данных и назначение привилегий. Эти задачи выполняются **только на Master-узле** (`when: is_master | bool`), так как на репликах БД находится в режиме Read-Only.

## Example Playbook

```yaml
- hosts: db_cluster
  roles:
    - role: postgresql
      vars:
        is_master: "{{ inventory_hostname == 'db-01' }}"
        pg_repl_password: "{{ vault_pg_repl_password }}"
        gitea_db_password: "{{ vault_gitea_db_password }}"
```

## Structure Notes

Для корректной работы задач Gitea убедитесь, что `tasks/main.yml` включает в себя `gitea.yml`:
```yaml
- include_tasks: gitea.yml
  tags: [gitea, db]
```

## Technical Details
- **Идемпотентность**: Создание объектов БД проверяет наличие существующих записей.
- **Безопасность**: Для управления базой используется `become_user: postgres`.
- **Репликация**: Настройки в `postgresql.conf.j2` адаптированы под `hot_standby`.


