## keepalived

Обеспечивает высокую доступность (L3 HA) через виртуальный IP (VIP) с использованием протокола VRRP в режиме Unicast.

## Requirements

- **Ansible** >= 2.14
- **OS**: Ubuntu 22.04
- **Network**: Прохождение IP-протокола 112 (VRRP) между узлами.

## Role Variables

| Variable | Default | Description |
| :--- | :--- | :--- |
| `pg_vip` | "172.20.0.10" | Виртуальный IP-адрес кластера |
| `pg_netmask` | "24" | Маска подсети для VIP |
| `keepalived_auth_pass` | "amikon8" | Пароль аутентификации VRRP (строго 8 символов) |
| `keepalived_priority` | 100 | Базовый приоритет узла в VRRP-выборах |

## Dependencies

- `postgresql` (функциональная зависимость для скрипта `check_pg`).

## Example Playbook

```yaml
- hosts: db_cluster
  roles:
    - role: keepalived
      vars:
        keepalived_priority: "{{ 110 if inventory_hostname == 'db-01' else 100 }}"
```

## Notes
- Использует `Unicast` для стабильной работы в Docker Bridge сетях.
- Включает `enable_script_security` и запуск чеков от пользователя `postgres`.
- Алгоритм Failover: штраф `-20` к приоритету при падении СУБД.
