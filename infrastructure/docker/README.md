## Компоненты
1. **docker-compose.yaml**: Описывает топологию сети, тома данных и параметры запуска 10 узлов кластера.
2. **node-ubuntu.Dockerfile**: Описывает образ на базе Ubuntu 22.04.
3. **.env**: Файл с переменными окружения 

Все контейнеры объединены в bridge-сеть `lab-internal-net` (подсеть `172.20.0.0/24`).

| Роль в кластере | Имена узлов | Статические IP |
| --- | --- | --- |
| **Database** | `db-node-01`, `db-node-02` | `.21`, `.22` |
| **Application** | `gitea-node-01`, `gitea-node-02` | `.11`, `.12` |
| **Load Balancer** | `gitea-proxy-01`, `gitea-proxy-02` | `.15`, `.16` |
| **Storage** | `storage-01`, `storage-02` | `.41`, `.42` |
| **Monitoring** | `monitor-node-01` | `.50` |
| **Backup** | `backup-srv` | `.60` |

## Особенности запуска

Для корректной работы `systemd` и `Keepalived` (протокол VRRP) контейнеры запускаются со следующими параметрами:

* **privileged**: `true` — требуется для монтирования cgroups и управления сетевым стеком.
* **volumes**: Проброс `/sys/fs/cgroup:/sys/fs/cgroup:rw`.

## Инструкция по эксплуатации

### Запуск стенда

```bash
docker-compose up -d

```

### Проверка доступности узлов для Ansible

```bash
docker exec -it monitor-node-01 ping db-node-01

```

### Очистка 

```bash
docker-compose down -v

```