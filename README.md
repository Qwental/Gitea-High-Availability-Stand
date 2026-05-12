# GitOps Gitea High-Availability Stand

Репозиторий для автоматического развертывания отказоустойчивого Gitea.


**[Демонстрация отказоустойчивости и Observability](</docs/demo.md>)**

[Пример условия тестового задания](</docs/Пример условия тестового задания.md>)

## 1. Архитектура системы
![Архитектура кластера](/docs/images/Архитектура.png)

Система представляет собой многоуровневый кластер:
* **Proxy Layer:** HAProxy + Keepalived для балансировки HTTP/SSH
* **App Layer:** Несколько нод Gitea.
* **Database Layer:** PostgreSQL (Master-Replica) + repmgr + Keepalived.
* **Storage Layer:** NFS Ganesha + Keepalived для общих репозиториев.
* **Слой мониторинга:** Prometheus, Loki, Grafana, Alertmanager.
* **Слой бэкапов:** Restic Server + Agents.

## 2 Как запустить?

## 2.1 Подготовка инфраструктуры
настройте `.env` по примеру `.env.example`
```bash
    cd infrastructure/docker
    docker-compose up -d
 ```
## 2.2 Настройка переменных Ansible

Переменные окружения все находятся в `infrastructure/ansible/group_vars/`.

есть `vault.yml.example` в котором описано какие секреты должны быть помещены в `infrastructure/ansible/group_vars/vault.yml` , можно просто скопировать и вставить если не важны алерты на почту

### 2.3 Шифрование секретов(опционально)
Для работы необходимо создать файл с паролем для Vault (например, `.vault_pass`) и зашифровать конфиденциальные данные:
```bash
    # Отредактируйте файлы и сохраните
    ansible-vault encrypt infrastructure/ansible/group_vars/all/vault.yml
    ansible-vault encrypt infrastructure/ansible/group_vars/backup/vault.yml
```

### 2.4 Основные переменные и настройки подключения

Настройки среды в инвентаре (`infrastructure/ansible/inventory.ini`)
В блоке `[all:vars]` задаются параметры доступа и архитектура хоста
`target_arch`: Укажите `arm64` или `amd64` 


##  2.5. Развертывание через ansible
Желательно включить впнчик с иностранным сервером =)
Запуск полной цепочки установки всех ролей: 

можно без впна:
```bash
ansible-playbook -i inventory.ini deploy.yml --vault-password-file .vault_pass --skip-tags monitoring
```

Обязательно с впном(зеркала яндекса почему-то не работают, поэтому только так)
```bash
ansible-playbook -i inventory.ini deploy.yml --vault-password-file .vault_pass --tags monitoring
```


```bash
cd infrastructure/ansible
ansible-playbook -i inventory.ini deploy.yml --vault-password-file .vault_pass

```

##  2.6. Проверка работоспособности

Топология доступа к web-интерфейсам зависит от хостовой ОС. Docker Desktop (macOS/Windows) не маршрутизирует внутренние IP-адреса контейнеров на хост, поэтому используется `localhost` с проброшенными портами. В Linux доступна прямая маршрутизация.

**Доступ для macOS / Windows (Port Mapping):**
* **Gitea:** `http://localhost:3000` (проксируется через `gitea-proxy-01`) или `http://localhost:3002` (через `gitea-proxy-02`).
* **Grafana:** `http://localhost:3001`.
* **Prometheus:** `http://localhost:9090`.

**Доступ для Linux (Native Bridge Routing):**
* **Gitea:** `http://172.20.0.20:3000` (обращение напрямую к VIP-адресу кластера балансировщиков).
* **Grafana:** `http://172.20.0.50:3000` (обращение напрямую к IP-адресу узла мониторинга).
* **Prometheus:** `http://172.20.0.50:9090`.

* **Статус кластера БД:** 
```bash
  docker exec -it db-node-01 su - postgres -c "repmgr cluster show"
```


---

**Автор:** Бугренков Владимир

