# Описание роли: keepalived

Роль предназначена для установки и базовой настройки демона Keepalived. Обеспечивает отказоустойчивость (High Availability) на сетевом уровне (L2/L3) путем управления виртуальными IP-адресами (VIP) по протоколу VRRP.

В рамках данной архитектуры роль является универсальной (cross-layer) и применяется для кластеров БД (PostgreSQL), балансировщиков (HAProxy) и хранилища (NFS).

## Требования (Requirements)

* **ОС**: Ubuntu/Debian (используется `apt`).
* **Docker/Контейнеризация**: Контейнеры, на которых запускается Keepalived, **обязаны** иметь привилегии (capabilities) `NET_ADMIN` и `NET_RAW` для управления сетевыми интерфейсами и отправки VRRP-пакетов.
* **Сеть**: В Docker bridge-сетях multicast-трафик VRRP часто блокируется. Ожидается использование unicast-конфигурации (`unicast_peer`) в шаблоне конфигурации.

## Переменные роли (Role Variables)

Точный список зависит от содержимого `keepalived.conf.j2`. На основе стандартных паттернов развертывания ожидаются следующие переменные (определяются в `host_vars` или `group_vars`):

| Переменная (ожидаемая) | Описание |
| --- | --- |
| `keepalived_state` | Состояние узла по умолчанию (`MASTER` или `BACKUP`). |
| `keepalived_interface` | Сетевой интерфейс для привязки VIP (например, `eth0`). |
| `keepalived_router_id` | Уникальный идентификатор виртуального маршрутизатора (VRID, 1-255). Должен быть одинаковым для узлов одного кластера и уникальным в пределах L2-сегмента. |
| `keepalived_priority` | Приоритет узла (например, `100` для MASTER, `90` для BACKUP). |
| `keepalived_vip` | Назначаемый виртуальный IP-адрес (например, `172.20.0.20`). |
| `keepalived_auth_pass` | Пароль для аутентификации VRRP-пакетов (опционально, но рекомендуется). |


## Взаимодействие с другими компонентами

* **HAProxy (Proxy Layer)**: Keepalived обеспечивает доступность балансировщика. При падении основного прокси VIP переезжает на резервный.
* **PostgreSQL (DB Layer)**: Keepalived держит VIP базы данных. *Риск:* Если Keepalived переключит VIP, но primary-нода БД фактически не упала (split-brain), возможна запись в обе ноды. Требуется строгая интеграция с `repmgr` (notify-скрипты) для синхронного переключения ролей БД и VIP.
* **NFS Ganesha (Storage Layer)**: Обеспечивает единую точку монтирования (`storage_vip`) для Gitea.

## Пример использования (Example Playbook)

```yaml
- name: Configure Keepalived for HAProxy
  hosts: gitea_proxies
  become: yes
  vars:
    keepalived_interface: eth0
    keepalived_vip: "172.20.0.20"
    keepalived_router_id: 51
  roles:
    - role: keepalived
      tags: keepalived

```