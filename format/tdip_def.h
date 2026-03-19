#ifndef TDIP_DEF_H_20260128
#define TDIP_DEF_H_20260128

#ifdef __cplusplus
extern "C" {
#endif

typedef enum LimitType{
    LIMIT_TYPE_APP = 0, //视电阻率(Ω·m)
    LIMIT_TYPE_APPC,  //充电率（mV/V）
    LIMIT_TYPE_CNT,
}LimitType_N;

#ifdef __cplusplus
}
#endif
#endif // TDIP_DEF_H_20260128
