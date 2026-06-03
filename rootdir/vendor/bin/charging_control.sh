#!/vendor/bin/sh
# Charging control script for Xiaomi dada
# Reads limit from LineageSettings and writes correct soc_limit command

ACTION="$1"
SMART_BATT="/sys/devices/platform/soc/soc:smart_charge/smart_batt"
DEBUG_CTRL="/sys/devices/platform/soc/soc:mca_business_charger/debug_ctrl"

get_limit() {
    # Try to read from LineageSettings, default to 80
    LIMIT=$(settings get lineage_system charging_control_charging_limit 2>/dev/null)
    if [ -z "$LIMIT" ] || [ "$LIMIT" = "null" ]; then
        LIMIT=80
    fi
    echo "$LIMIT"
}

case "$ACTION" in
    disable)
        LIMIT=$(get_limit)
        echo 1 > "$SMART_BATT"
        echo "soc_limit 1 $LIMIT" > "$DEBUG_CTRL"
        ;;
    enable)
        echo 0 > "$SMART_BATT"
        echo "soc_limit 0 0" > "$DEBUG_CTRL"
        ;;
    *)
        echo "Usage: $0 {enable|disable}"
        exit 1
        ;;
esac
