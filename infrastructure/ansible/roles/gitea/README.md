# Описание роли: gitea

Роль предназначена для идемпотентного развертывания Gitea в режиме Active-Passive/Active-Active с использованием внешнего NFS-хранилища и базы данных.

## Требования (Requirements)

* **ОС**: Ubuntu/Debian (используется модуль `ansible.builtin.apt`).
* **Зависимости коллекций**: `ansible.posix` (для модуля `mount`).
* **Инфраструктура**:
* Доступный NFS-сервер (определяется переменной `storage_vip`).
* Доступная база данных (определяется переменными `gitea_db_*`).
* Доступ в интернет для загрузки бинарного файла Gitea.

### Основные настройки приложения

| Переменная | Описание |
| --- | --- |
| `gitea_version` | Версия Gitea для загрузки (например, `1.21.0`). |
| `target_arch` | Архитектура бинарного файла (например, `amd64`, `arm64`). |
| `gitea_user` | Системный пользователь для запуска Gitea. |
| `gitea_group` | Системная группа для запуска Gitea. |
| `gitea_home` | Домашняя директория пользователя Gitea. |
| `gitea_work_dir` | Рабочая директория (обычно `/var/lib/gitea`). |
| `gitea_conf_dir` | Директория конфигурации (обычно `/etc/gitea`). |
| `gitea_domain` | FQDN или IP-адрес для `DOMAIN` и `SSH_DOMAIN`. |
| `gitea_http_port` | Порт, на котором Gitea слушает HTTP-запросы (по умолчанию 3000 в tasks). |
| `gitea_root_url` | Полный URL для доступа к Gitea (используется в `ROOT_URL`). |

### Настройки интеграции

| Переменная | Описание |
| --- | --- |
| `storage_vip` | IP-адрес или hostname NFS-сервера для монтирования `/var/lib/gitea/data/gitea-repositories`. |
| `gitea_db_type` | Тип базы данных (например, `postgres`). |
| `gitea_db_host` | Хост и порт базы данных (например, `172.20.0.10:5432`). |
| `gitea_db_name` | Имя базы данных Gitea. |
| `gitea_db_user` | Пользователь базы данных. |

### Секреты (должны храниться в Ansible Vault)

| Переменная | Описание |
| --- | --- |
| `gitea_db_password` | Пароль от базы данных. |
| `vault_gitea_secret_key` | Секретный ключ для конфигурации (`SECRET_KEY`). |
| `vault_gitea_internal_token` | Внутренний токен Gitea (`INTERNAL_TOKEN`). |
| `vault_gitea_jwt_secret` | Секрет для OAuth2 JWT (`JWT_SECRET`). |
| `vault_gitea_admin_username` | Логин первичного администратора. |
| `vault_gitea_admin_password` | Пароль первичного администратора. |
| `vault_gitea_admin_email` | Email первичного администратора. |


## Пример использования (Example Playbook)

```yaml
- name: Deploy Gitea Application
  hosts: gitea_servers
  become: true
  vars_files:
    - vault.yml
  roles:
    - role: gitea
      tags: gitea

```