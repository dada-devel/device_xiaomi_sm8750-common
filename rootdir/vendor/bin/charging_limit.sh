#!/vendor/bin/sh
# Xiaomi SM8750 charging limit control
# Usage: charging_limit.sh <0|percentage>
# 0 = enable charging, 1-100 = set limit percentage

SMART_BATT="/sys/devices/platform/soc/soc:smart_charge/smart_batt"
DEBUG_CTRL="/sys/devices/platform/soc/soc:mca_business_charger/debug_ctrl"

case "$1" in
    0)
        echo 0 > "$SMART_BATT"
        echo "soc_limit 0 0" > "$DEBUG_CTRL"
        ;;
    *)
        echo 1 > "$SMART_BATT"
        echo "soc_limit 1 $1" > "$DEBUG_CTRL"
        ;;
esac
