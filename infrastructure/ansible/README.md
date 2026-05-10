# Ansible Infrastructure Management

## Архитектурный стек

* **Proxy Layer**: HAProxy + Keepalived (VIP).
* **Application Layer**: Gitea (NFS client).
* **Database Layer**: PostgreSQL (Master-Replica) + `repmgr` + Keepalived (VIP).
* **Storage Layer**: NFS Ganesha + Keepalived (VIP).
* **Observability**: Prometheus, Loki, Grafana, Alertmanager.
* **Backup**: Restic (REST Server + Agents).

## Структура репозитория

* `deploy.yml` — основной плейбук
* `inventory.ini` — описание узлов и параметров подключения.
* `group_vars/` — переменные групп (включая зашифрованные секреты в `vault.yml`).
* `roles/` — модульные компоненты системы.

## Требования (Prerequisites)

1. **Ansible**: версии 2.12+.
2. **Секреты**: Наличие пароля к Vault в файле `.vault_pass`.
3. **SSH**: Настроенный доступ по ключам к контейнерам (узлам).
4. **Коллекции**:
```bash
ansible-galaxy collection install ansible.posix community.postgresql

```

## Порядок развертывания (Deployment Order)

Оркестрация в `deploy.yml` строго типизирована для соблюдения зависимостей:
1. **Storage**: Подготовка общего хранилища (необходимо для старта Gitea).
2. **Database**: Подготовка кластера БД (необходимо для Gitea).
3. **Load Balancer**: Настройка HAProxy для маршрутизации трафика.
4. **Application**: Деплой Gitea.
5. **Observability**: Раскатка агентов и сервера мониторинга.
6. **Backups**: Настройка Restic Server и расписаний на агентах.

Запуск полной установки:

```bash
ansible-playbook -i inventory.ini deploy.yml --vault-password-file .vault_pass

```
