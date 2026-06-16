第23組 楊采昕 陳亮詠

### 系統軟體流程圖


```mermaid
flowchart TD
    classDef init fill:#e1f5fe,stroke:#01579b,stroke-width:2px;
    classDef loop fill:#fff9c4,stroke:#fbc02d,stroke-width:2px;
    classDef vls fill:#ffecb3,stroke:#ff6f00,stroke-width:2px;
    classDef ebs fill:#ffcdd2,stroke:#b71c1c,stroke-width:2px;
    classDef track fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px;

    START([開始 START]) --> SETUP[① 初始化 setup]:::init
    SETUP --> |亮藍燈 / 延時1秒| LOOP[② 主迴圈開始 loop]:::loop
    
    LOOP --> READ[讀取感測器 s1 ~ s5]:::loop
    READ --> CHK_VLS{turnLock 狀態?}:::vls
    
    CHK_VLS -->|turnLock == -1 左鎖定| VLS_L{s3==0 或 s2==0?}:::vls
    VLS_L -->|Yes: 抓回中心線| UNLOCK_L[turnLock = 0 解鎖]:::vls
    VLS_L -->|No: 繼續盲轉| GO_VLS_L[亮紅燈 / sharpLeft / delay 5ms]:::vls
    GO_VLS_L --> RETURN_1([返回主迴圈]) --> LOOP
    
    CHK_VLS -->|turnLock == 1 右鎖定| VLS_R{s3==0 或 s4==0?}:::vls
    VLS_R -->|Yes: 抓回中心線| UNLOCK_R[turnLock = 0 解鎖]:::vls
    VLS_R -->|No: 繼續盲轉| GO_VLS_R[亮紅燈 / sharpRight / delay 5ms]:::vls
    GO_VLS_R --> RETURN_2([返回主迴圈]) --> LOOP
    
    CHK_VLS -->|turnLock == 0 無鎖定| UNLOCK_L
    CHK_VLS -->|turnLock == 0 無鎖定| UNLOCK_R
    
    UNLOCK_L & UNLOCK_R --> CHK_EBS_STOP{s1~s5 皆為 0?<br>EBS 衝線煞車}:::ebs
    
    CHK_EBS_STOP -->|Yes| ACTION_STOP[亮綠燈 / stopCar<br>重設所有變數]:::ebs
    ACTION_STOP --> DELAY[⑤ 迴圈尾端延時 delay 5ms]:::loop
    
    CHK_EBS_STOP -->|No| CHK_S1{s1 == 0?<br>大左轉觸發}:::track
    CHK_S1 -->|Yes| ACT_S1[亮紅燈 / sharpLeft / delay 20ms<br>啟動 VLS: turnLock = -1]:::vls
    
    CHK_S1 -->|No| CHK_S5{s5 == 0?<br>大右轉觸發}:::track
    CHK_S5 -->|Yes| ACT_S5[亮紅燈 / sharpRight / delay 20ms<br>啟動 VLS: turnLock = 1]:::vls
    
    CHK_S5 -->|No| CHK_S2{s2 == 0?<br>小左轉}:::track
    CHK_S2 -->|Yes| ACT_S2[亮紅燈 / left 弧度彎]:::track
    
    CHK_S2 -->|No| CHK_S4{s4 == 0?<br>小右轉}:::track
    CHK_S4 -->|Yes| ACT_S4[亮紅燈 / right 弧度彎]:::track
    
    CHK_S4 -->|No| CHK_S3{s3 == 0?<br>直行}:::track
    CHK_S3 -->|Yes| ACT_S3[亮紅燈 / forward]:::track
    
    CHK_S3 -->|No: 全懸空迷路| CHK_RESCUE{lastDirection<br>最後記憶方向?}:::ebs
    CHK_RESCUE -->| -1 | RES_L[亮紅燈 / sharpLeft 搜線]:::ebs
    CHK_RESCUE -->| 1 | RES_R[亮紅燈 / sharpRight 搜線]:::ebs
    CHK_RESCUE -->| 0 | RES_STOP[亮綠燈 / stopCar 安全停]:::ebs
    
    ACT_S1 & ACT_S5 & ACT_S2 & ACT_S4 & ACT_S3 & RES_L & RES_R & RES_STOP --> DELAY
    DELAY --> LOOP
