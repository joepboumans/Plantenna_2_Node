/*
* Plantenna 2 node - bt mesh sensor setup server
* File name:    sensor_setup_server.c
* Author:       Frank Arts
* Date:         26-01-2021
* Version:      V1.1
* Version info
* - Added device data
*/

/* C standard includes */
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/* BT MESH includes */
#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>
#include <settings/settings.h>
#include <bluetooth/mesh/proxy.h>
#include <bluetooth/mesh/access.h>

/* BT MESH Sensor Model includes */
#include "mesh_base_model.h"
#include "mesh_sensor_setup_server.h"
#include "mesh_sensor_common.h"

// Variables
bool publish = false;
uint16_t reply_addr;
uint8_t reply_net_idx;
uint8_t reply_app_idx;
uint8_t reply_send_ttl;
struct bt_mesh_model *reply_model;


// -------------------------------------------------------------------------------------------------------
// Device data
// NOTE: Only one sensor/index can be send in a single message, except for Data.
// --------------------------
// Descriptor
sensor_descriptor_state_global_t  sensor_decriptor_global[no_sensors];

// Data
sensor_data_state_single_global_t sensor_data_global[no_sensors];

// Column
sensor_column_state_global_t      sensor_column_global[no_sensors];

// Series
sensor_series_state_global_t      sensor_series_global[no_sensors];

// Cadence
sensor_cadence_state_global_t     sensor_cadence_global[no_sensors];

// Settings (??? -> is this for a sensor with multiple sensors, like the BME sensor?)
sensor_settings_state_global_t    sensor_settings_global[no_sensors];

// Setting
sensor_setting_state_global_t     sensor_setting_global[no_sensors];

// -------------------------------------------------------------------------------------------------------
// Sensor functions
// --------------------------
// < sensor functions > //

// -------------------------------------------------------------------------------------------------------
// Sensor Setup Server Model
// --------------------------
// Forward declarations of tx functions
int sensor_cadence_status_tx();
int sensor_settings_status_tx();
int sensor_setting_status_tx();

// sensor server - handler functions for this model's RX messages
// Cadence //
void sensor_cadence_get_rx(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    // Not implemented yet
    printk("Sensor Cadence Get not implemented yet\n");
    return;
}

void sensor_cadence_set_rx(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    // Not implemented yet
    printk("Sensor Cadence Set not implemented yet\n");
    return;
}

void sensor_cadence_set_unack_rx(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    // Not implemented yet
    printk("Sensor Cadence Set Unack not implemented yet\n");
    return;
}

// Settings //
void sensor_settings_get_rx(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    // Not implemented yet
    printk("Sensor Settings Get not implemented yet\n");
    return;
}

// Setting //
void sensor_setting_get_rx(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    // Not implemented yet
    printk("Sensor Settting Get not implemented yet\n");
    return;
}

void sensor_setting_set_rx(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    // Not implemented yet
    printk("Sensor Settting Set not implemented yet\n");
    return;
}

void sensor_setting_set_unack_rx(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    // Not implemented yet
    printk("Sensor Settting Set Unack not implemented yet\n");
    return;
}

// Descriptor, Data, Column and Series in Sensor Server Model

// sensor_setup_srv_op[] in header
// sensor_setup_srv in header


// -------------------------------------------------------------------------------------------------------
// Sensor Server Model
// --------------------------
// Forward declarations of tx functions
int sensor_descriptor_status_tx(bool publish, sensor_descriptor_status_msg_pkt_t status, bool is_multiple_sensors, bool only_sensor_property_id);
int sensor_data_status_tx(struct bt_mesh_msg_ctx *ctx, uint16_t prop_id);
int sensor_column_status_tx();
int sensor_series_status_tx();

// sensor server - handler functions for this model's RX messages
// Descriptor //
void sensor_descriptor_get_rx(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    bool only_sensor_property_id = true;
    
    if (model->id != (uint16_t)BT_MESH_MODEL_ID_SENSOR_SRV)
    {
        printk("Model id %d, is not %d\n", model->id, BT_MESH_MODEL_ID_SENSOR_SRV);
        return;
    }
    
    // Print for debug purposes + save
    printk("ctx net_idx=0x%02x\n", ctx->net_idx);
    printk("ctx app_idx=0x%02x\n", ctx->app_idx);
    printk("ctx addr=0x%02x\n",    ctx->addr);
    printk("ctx recv_dst=0x%02x\n",ctx->recv_dst);
    
    reply_addr     = ctx->addr;
    reply_net_idx  = ctx->net_idx;
    reply_app_idx  = ctx->app_idx;
    reply_send_ttl = ctx->send_ttl;
    
    // Print for debug purposes
    printk("Message: %x. Property ID field present (1) or omitted (0)\n", *buf->data);
	bool is_multiple_sensors = *buf->data;
    sensor_descriptor_status_msg_pkt_t (*sensor_descriptor_status_reply_msg)[no_sensors] = &sensor_decriptor_global;
    
    // Send sensor_descriptor_status_tx messages
    int err = sensor_descriptor_status_tx(true, **sensor_descriptor_status_reply_msg, is_multiple_sensors, only_sensor_property_id);
    
    if (err)
    {
        printk("Sensor Descriptor Get rx processing failed with error %d", err);
        return;
    }
    
    // In test phase
    printk("Sensor Descriptor Get processed without errors\n");
    return;
}

// Data //
void sensor_data_get_rx(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    uint16_t prop_id = net_buf_simple_pull_le16(buf);
    printk("Received prop id: %d\n", prop_id);

    if(!prop_id || prop_id < 0x800) {
        printk("Invalid property_id");
        return;
    }

    if(!sensor_data_status_tx(ctx, prop_id)) {
        printk("Sensor Data Get processed without errors\n");
    }
    else {
        printk("Sensor Data Get processed with errors\n");
    }
}

// Column //
void sensor_column_get_rx(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    // Not implemented yet
    printk("Sensor Column Get not implemented yet\n");
    return;
}

// Series //
void sensor_series_get_rx(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf)
{
    // Not implemented yet
    printk("Sensor Series Get not implemented yet\n");
    return;
}

// Cadence, Settings and Setting in Sensor Setup Server Model

// sensor_srv_op[] in header
// sensor_srv in header

// -------------------------------------------------------------------------------------------------------
// Sensor Setup Server - TX message producer functions
// -----------------------------------------------------------
// Cadence
int sensor_cadence_status_tx()
{
    // Not implemented yet
    printk("Sensor Cadence Status not implemented yet\n");
    return 0;
}

// Settings
int sensor_settings_status_tx()
{
    // Not implemented yet
    printk("Sensor Settings Status not implemented yet\n");
    return 0;
}

// Setting
int sensor_setting_status_tx()
{
    // Not implemented yet
    printk("Sensor Setting Status not implemented yet\n");
    return 0;
}

// Descriptor, Data, Column and Series in Sensor Server - TX message producer functions

// -------------------------------------------------------------------------------------------------------
// Sensor Server - TX message producer functions
// -----------------------------------------------------------
// Descriptor
int sensor_descriptor_status_tx(bool publish, sensor_descriptor_status_msg_pkt_t status, bool is_multiple_sensors, bool only_sensor_property_id)
{
    struct bt_mesh_model *model = &sig_models[3];    // Use sensor_server model
    
    if (publish && model->pub->addr == BT_MESH_ADDR_UNASSIGNED)
    {
        printk("No publish address associated with the generic on off server model - add one with a configuration app like nRF Mesh\n");
        return bt_mesh_PUBLISH_NOT_SET;
    }
    
    if (publish)
    {
        struct net_buf_simple *msg = model->pub->msg;
        net_buf_simple_reset(msg);
        bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS);
        if (!is_multiple_sensors)    // Not 0 -> send for single sensor
        {
            if (only_sensor_property_id)    // ID only | all? -> only send Sensor Property ID | send all
            {
                net_buf_simple_add_mem(msg, &status.short_pkt, sizeof(status.short_pkt));
            }
            else
            {
                net_buf_simple_add_mem(msg, &status.full_pkt, sizeof(status.full_pkt));
            }
        }
        else    // 0 -> send for all sensors
        {
            if (only_sensor_property_id)    // ID only | all? -> only send Sensor Property ID | send all
            {
                net_buf_simple_add_mem(msg, &status.short_pkt, sizeof(status.short_pkt));
            }
            else
            {
                net_buf_simple_add_mem(msg, &status.full_pkt, sizeof(status.full_pkt));
            }
        }
        
        printk("Publishing descriptor status message...\n");
        int err = bt_mesh_model_publish(model);
        
        if (err)
        {
            printk("bt_mesh_model_publish err %d\n", err);
            return bt_mesh_PUBLISH_FAILED;
        }
    }
    else
    {
        uint8_t buflen = sizeof(status.full_pkt);
        NET_BUF_SIMPLE_DEFINE(msg, buflen);
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS);
        
        if (only_sensor_property_id)    // single | all? -> only send Sensor Property ID | send all
        {
            buflen = sizeof(status.short_pkt);
            
            if (status.short_pkt.sensor_property_id)    // Not 0 -> send for single sensor
            {
                net_buf_simple_add_mem(&msg, &status.short_pkt, buflen);
            }
            else
            {
                /* ADD: for multiple sensors */
            }
        }
        else
        {
            if (status.full_pkt.sensor_property_id)    // Not 0 -> send for single sensor
            {
                net_buf_simple_add_mem(&msg, &status.full_pkt, buflen);
            }
            else
            {
                /* ADD: for multiple sensors */
            }
        }
        
        struct bt_mesh_msg_ctx ctx = {
            .net_idx  = reply_net_idx,
            .app_idx  = reply_app_idx,
            .addr     = reply_addr,
            .send_ttl = reply_send_ttl,
            };
    
        printk("Sending descriptor status message...\n");
        
        int err = bt_mesh_model_send(model, &ctx, &msg, NULL, NULL);
        
        if (err)
        {
            printk("Unable to send descriptor status message. Error: %d\n", err);
            return bt_mesh_SEND_FAILED;
        }
    }
    
    // In test phase
    printk("Sensor Descriptor Status message published/send without errors.\n");
    return bt_mesh_SUCCEESS;
}

// Data
int sensor_data_status_tx(struct bt_mesh_msg_ctx *ctx, uint16_t prop_id)
{
    struct bt_mesh_model *model = &sig_models[3];    // Use sensor_server model
	struct net_buf_simple *msg = model->pub->msg;
	bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_SENSOR_DATA_STATUS);

    airflow_local sensor_data;
    get_sensor_series_index(get_local_storage_index() - 1, &sensor_data);

    uint16_t marshalled_id = prop_id ^ 0x2000;
    uint16_t sensor_raw;

    switch(prop_id) {
        case sensor_airflow_property_id:
            sensor_raw = sensor_data.airf;
            break;
        case sensor_bme_tmp_property_id:
            sensor_raw = sensor_data.temp;
            break;
        case sensor_bme_humid_property_id:
            sensor_raw = sensor_data.humi;
            break;
        case sensor_bme_pres_property_id:
            sensor_raw = sensor_data.pres;
            break;
        default:
            break;
    }

    printk("Val marshall: %d\n", marshalled_id);
    printk("Val sensor: %d\n", sensor_raw);

    unsigned payload = ((unsigned)marshalled_id << 16) | (unsigned)sensor_raw;
    printk("Created payload: %d\n", payload);

    net_buf_simple_add_le32(msg, payload);

    if(!bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        printk("Sensor data status message published/send without errors.\n");
        return bt_mesh_SUCCEESS;
    } 
    else {
        printk("Sensor data status message published/send with errors.\n");
        return bt_mesh_SEND_FAILED;
    }
}

// Column
int sensor_column_status_tx()
{
    // Not implemented yet
    printk("Sensor Column Status not implemented yet\n");
    return 0;
}

// Series
int sensor_series_status_tx() 
{
    // Not implemented yet
    printk("Sensor Series Status not implemented yet\n");
    return 0;
}

// Cadence, Settings and Setting in Sensor Setup Server - TX message producer functions
