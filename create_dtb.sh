cp ./tegra124-pm375.dts /boot/tegra124-pm375.dts
cd /usr/src/kernel
./scripts/dtc/dtc -I dts -O dtb -o /boot/tegra124-pm375.dtb /boot/tegra124-pm375.dts

