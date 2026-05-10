# Ansible Infrastructure Management

## Архитектурный стек

* **Proxy Layer**: HAProxy + Keepalived (VIP).
* **Application Layer**: Gitea (NFS client).
* **Database Layer**: PostgreSQL (Master-Replica) + `repmgr` + Keepalived (VIP).
* **Storage Layer**: NFS Ganesha + Keepalived (VIP).
* **Observability**: Prometheus, Loki, Grafana, Alertmanager.
* **Backup**: Restic (REST Server + Agents).

## Структура

* `deploy.yml` — основной плейбук
* `inventory.ini` — описание узлов и параметров подключения.
* `group_vars/` — переменные групп (включая зашифрованные секреты в `vault.yml`) для примера сейчас в нем "1234".
* `roles/` — модульные компоненты системы.

## Требования

1. **Ansible**: версии 2.12+.
2. **Секреты**: Наличие пароля к Vault в файле `.vault_pass`.
3. **Коллекции**:
```bash
ansible-galaxy collection install ansible.posix community.postgresql
```

Запуск полной установки:

```bash
ansible-playbook -i inventory.ini deploy.yml --vault-password-file .vault_pass
```
