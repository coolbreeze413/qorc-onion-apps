#ifndef INC_QL_SENSORHUB_POLL_TASK_H_
#define INC_QL_SENSORHUB_POLL_TASK_H_

#include <stdint.h>

// note: maintain 32b alignment for all structures to avoid unncessary logic changes

// config values for bmx160 sensor poll data
struct sensorhub_poll_config_bmx160 {
  
  uint16_t bmx160_ts_range; // replace with scale instead.
  uint16_t bmx160_a_range;
  uint16_t bmx160_g_range;
  uint16_t bmx160_m_range;  
  uint16_t bmx160_t_range; // 1/range = scale
  uint16_t bmx160_h_range; // 1/range = scale, unknown for now.
};

// raw bmx160 sensor poll data
struct sensorhub_poll_data_bmx160r {
  
    uint32_t bmx160_ts;         // timestamp 24b
    int16_t bmx160_a_x;
    int16_t bmx160_a_y;
    int16_t bmx160_a_z;
    uint16_t bmx160_a_f1;       // filler
    int16_t bmx160_g_x;
    int16_t bmx160_g_y;
    int16_t bmx160_g_z;
    uint16_t bmx160_g_f1;       // filler
    int16_t bmx160_m_x;
    int16_t bmx160_m_y;
    int16_t bmx160_m_z;
    uint16_t bmx160_m_f1;       // filler
    int16_t bmx160_h;          // rhall 16b
    int16_t bmx160_t;          // temperature 16b 
};

// unit converted bmx160 sensor poll data
struct sensorhub_poll_data_bmx160 {
  
  uint32_t bmx160_ts;
  double bmx160_a_x;
  double bmx160_a_y;
  double bmx160_a_z;
  double bmx160_g_x;
  double bmx160_g_y;
  double bmx160_g_z;
  double bmx160_m_x;
  double bmx160_m_y;
  double bmx160_m_z;
  uint32_t bmx160_h; // temporary, need to convert
  double bmx160_t; 
};

struct sensorhub_poll_config_dps310 {
  
    // coefficients
    int16_t _c0, _c1, _c01, _c11, _c20, _c21, _c30, _f1;
    int32_t _c00, _c10;
    
    uint8_t _coeff_source_temp_sensor;
    uint8_t _selected_temp_sensor;
    uint8_t _temp_measure_rate;
    uint8_t _temp_oversampling_rate;
    
    uint8_t _press_measure_rate;
    uint8_t _press_oversampling_rate;
    uint16_t _f2;
    
    uint32_t dps310_scaling_factors[8];    
};

// raw dps310 sensor poll data
struct sensorhub_poll_data_dps310r {
  
  int32_t dps310_temp;
  int32_t dps310_press;  
};

struct sensorhub_poll_data_dps310 {
  
  float dps310_temp;
  float dps310_press;  
};

struct sensorhub_poll_config_bma400 {
  
  uint16_t bma400_a_range;
  uint16_t bma400_a_bitwidth;
  uint32_t bma400_ts_range;
};

// raw bma400 sensor poll data
struct sensorhub_poll_data_bma400r {
  
  uint16_t bma400_a_x;
  uint16_t bma400_a_y;
  uint16_t bma400_a_z;
  uint16_t bma400_a_f1; // filler
  uint32_t bma400_ts;  
};

struct sensorhub_poll_data_bma400 {
  
  uint16_t tap_count;
  uint16_t gen1_count;
  uint16_t gen2_count;
  uint16_t orient_ch_count;
  uint16_t int_overrun_count;
  uint16_t activity;
  uint32_t step_count;  
  float bma400_a_x;
  float bma400_a_y;
  float bma400_a_z;
  float bma400_ts;
};

struct sensorhub_poll_config_hx3313 {
  
  uint8_t led_current;
  uint8_t ps_osr;
  uint8_t ps_led_on_time;
  uint8_t ps_phase_offset_idac;
  uint8_t ps_interval;
  uint8_t hrs_osr;
  uint8_t hrs_led_on_time;
  uint8_t hrs_phase_offset_idac;
};

struct sensorhub_poll_data_hx3313r {
  
  uint8_t interrupt_1;
  uint8_t interrupt_2;
  uint8_t interrupt_3;
  uint8_t f0;
  
  uint8_t hrs_data[3];
  uint8_t f1;
  
  uint8_t als_data1[3];
  uint8_t f2;
  uint8_t als_data2[3];
  uint8_t f3;
  uint8_t ps_data[3];
  uint8_t f4;
};

struct sensorhub_poll_data_hx3313 {
  
  uint32_t hr;
  uint32_t bp;
  
};

struct sensorhub_poll_config {
  
    struct sensorhub_poll_config_bmx160 bmx160_config;
    struct sensorhub_poll_config_dps310 dps310_config;
    struct sensorhub_poll_config_bma400 bma400_config;
    struct sensorhub_poll_config_hx3313 hx3313_config;
};

struct sensorhub_poll_data {
  
    struct sensorhub_poll_data_bmx160r bmx160r_data;
    struct sensorhub_poll_data_dps310r dps310r_data;
    struct sensorhub_poll_data_bma400r bma400r_data;
    struct sensorhub_poll_data_hx3313r hx3313r_data;
    
    struct sensorhub_poll_data_bmx160 bmx160_data;
    struct sensorhub_poll_data_dps310 dps310_data;
    struct sensorhub_poll_data_bma400 bma400_data;
    struct sensorhub_poll_data_hx3313 hx3313_data;
};



#endif // #ifndef INC_QL_SENSORHUB_POLL_TASK_H_