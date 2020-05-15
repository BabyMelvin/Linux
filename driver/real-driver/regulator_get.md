
# regulator使用

dts中配置regulator

```dts
mt_pmic_vcamio_ldo_reg: ldo_vcamio {
            compatible = "regulator-fixed";
            regulator-name = "vcamio";
            regulator-min-microvolt = <1800000>;                          regulator-max-microvolt = <1800000>;
            regulator-enable-ramp-delay = <264>;
      };

kd_camera_hw1:kd_camera_hw1@1a040000 {
         compatible = "mediatek,camera_hw";
         reg = <0 0x1a040000 0 0x1000>;  /* SENINF_ADDR */

        /* Camera Common Clock Framework (CCF) */
       clocks = <&topckgen TOP_MUX_CAMTG>,
       <&topckgen TOP_MUX_SCAM>,
       <&topckgen TOP_UNIVPLL_192M_D4>,
       <&topckgen TOP_UNIVPLL2_D2>;
       clock-names = "TOP_CAMTG_SEL","TOP_MUX_SCAM","TOP_UNIVPLL_192M_D4","TOP_UNIVPLL2_D2";
        vcamio-supply = <&mt_pmic_vcamio_ldo_reg>;
    };
```

获取和使能电压

```c
struct device *sensor_device = device_create(sensor_class, NULL, g_CAMERA_HWdevno, NULL, CAMERA_HW_DRVNAME1);

// 获取dts中的regulator-name = "vcamio"
struct regulator *regcvama = regulator_get(sensor_device, "vcamaio");

// 设置稳压范围
regulator_set_voltage(reg, powerVolt, powerVolt);
regulator_enable(reg);
```

* `struct regulator *regulator_get(struct device *dev, const char *id)`