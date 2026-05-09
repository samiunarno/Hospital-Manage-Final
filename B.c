#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>  

#define MAX_NAME_LEN 20
#define MAX_DEP_LEN 20
#define MAX_DIAG_LEN 100

void addIntoQueue(int registerId, int patientId, int doctorId);

// 病历记录结构体
typedef struct VisitRecord {
    int recordId;                  // 病历ID
    char diagnosis[MAX_DIAG_LEN];  // 诊断结果
    double cost;                   // 单次就诊费用
    struct VisitRecord* next;      // 链表下一个节点
} VisitRecord;

// 处方药品项结构体
typedef struct PrescriptionItem {
    int medicineId;
    char medicineName[MAX_NAME_LEN];
    int quantity;
    double itemCost;
    struct PrescriptionItem* next;
} PrescriptionItem;

// 处方结构体
typedef struct Prescription {
    int prescriptionId;
    int patientId;
    double totalCost;
    PrescriptionItem* items;
    struct Prescription* next;
} Prescription;

// 患者结构体
typedef struct Patient {
    int patientId;
    char name[MAX_NAME_LEN];
    int age;
    char gender;                   // M男/F女
    char department[MAX_DEP_LEN];   // 所属科室
    int doctorId;                   // 主治医生ID
    bool isHospitalized;            // 是否住院：true住院/false未住院
    int bedId;                      // 床位ID，未住院为-1
    double totalCost;               // 患者总费用
    VisitRecord* records;           // 患者病历链表头
    struct Patient* next;
} Patient;

// 医生结构体
typedef struct Doctor {
    int doctorId;
    char name[MAX_NAME_LEN];
    char department[MAX_DEP_LEN];    // 所属科室
    char title[MAX_NAME_LEN];        // 职称
    int patientCount;               // 接诊患者数
    struct Doctor* next;
} Doctor;

// 药品结构体
typedef struct Medicine {
    int medicineId;
    char name[MAX_NAME_LEN];
    double price;
    int stock;
    int warningLine;                // 库存预警线
    struct Medicine* next;
} Medicine;

// 床位结构体
typedef struct Bed {
    int bedId;
    char wardType[MAX_NAME_LEN];     // 病房类型（普通/重症）
    bool isOccupied;                 // 是否占用
    int patientId;                   // 占用患者ID，未占用为-1
    struct Bed* next;
} Bed;

// ===================== 挂号记录 Registration =====================
typedef struct Registration {
    int registerId;
    int patientId;
    int doctorId;
    char department[MAX_DEP_LEN];
    int registerType;   // 0现场挂号 1预约挂号
    int registerTime;   // 时间顺序号
    int status;         // 0候诊中 1已叫号 2就诊中 3已完成
    struct Registration* next;
} Registration;

// ===================== 候诊队列 QueueNode =====================
typedef struct QueueNode {
    int queueId;
    int registerId;
    int patientId;
    int doctorId;
    int status;
    struct QueueNode* next;
} QueueNode;

// 全局链表头
Patient* patientHead = NULL;
Doctor* doctorHead = NULL;
Medicine* medicineHead = NULL;
Bed* bedHead = NULL;
Prescription* prescriptionHead = NULL;

Registration* regHead = NULL;
QueueNode* queueHead = NULL;

// ===================== 科室&医生查询 =====================
void queryAllDepartment() {
    if (doctorHead == NULL) {
        printf("暂无医生数据，无法查询科室！\n");
        return;
    }
    char deptList[100][MAX_DEP_LEN];
    int deptCount = 0;
    Doctor* p = doctorHead;
    printf("=== 医院所有科室 ===\n");
    while (p != NULL) {
        bool exist = false;
        for (int i = 0; i < deptCount; i++) {
            if (strcmp(deptList[i], p->department) == 0) {
                exist = true;
                break;
            }
        }
        if (!exist) {
            strcpy(deptList[deptCount++], p->department);
            printf("%d. %s\n", deptCount, p->department);
        }
        p = p->next;
    }
}

void queryDoctorByDept(const char* deptName) {
    if (doctorHead == NULL) {
        printf("暂无医生数据！\n");
        return;
    }
    Doctor* p = doctorHead;
    int count = 0;
    printf("=== %s 科室医生列表 ===\n", deptName);
    while (p != NULL) {
        if (strcmp(p->department, deptName) == 0) {
            count++;
            printf("医生ID：%d | 姓名：%s | 职称：%s | 接诊人数：%d\n",
                   p->doctorId, p->name, p->title, p->patientCount);
        }
        p = p->next;
    }
    if (count == 0) printf("该科室暂无医生！\n");
}

Doctor* queryDoctorById(int doctorId) {
    Doctor* p = doctorHead;
    while (p != NULL) {
        if (p->doctorId == doctorId) return p;
        p = p->next;
    }
    return NULL;
}

// ===================== 患者就诊病历 =====================
VisitRecord* createVisitRecord(int recordId, const char* diagnosis, double cost) {
    VisitRecord* newNode = (VisitRecord*)malloc(sizeof(VisitRecord));
    newNode->recordId = recordId;
    strcpy(newNode->diagnosis, diagnosis);
    newNode->cost = cost;
    newNode->next = NULL;
    return newNode;
}

void addPatientVisit(int patientId, int recordId, const char* diagnosis, double cost) {
    Patient* p = patientHead;
    while (p != NULL) {
        if (p->patientId == patientId) {
            VisitRecord* newRecord = createVisitRecord(recordId, diagnosis, cost);
            if (p->records == NULL) {
                p->records = newRecord;
            } else {
                VisitRecord* r = p->records;
                while (r->next != NULL) r = r->next;
                r->next = newRecord;
            }
            p->totalCost += cost;
            Doctor* doc = queryDoctorById(p->doctorId);
            if (doc != NULL) doc->patientCount++;
            printf("就诊记录添加成功！患者总费用：%.2f\n", p->totalCost);
            return;
        }
        p = p->next;
    }
    printf("未找到该患者，就诊记录添加失败！\n");
}

void queryPatientRecord(int patientId) {
    Patient* p = patientHead;
    while (p != NULL) {
        if (p->patientId == patientId) {
            printf("=== 患者【%s】(ID:%d) 所有病历 ===\n", p->name, p->patientId);
            if (p->records == NULL) {
                printf("暂无病历记录！\n");
                return;
            }
            VisitRecord* r = p->records;
            while (r != NULL) {
                printf("病历ID：%d | 诊断：%s | 费用：%.2f\n",
                       r->recordId, r->diagnosis, r->cost);
                r = r->next;
            }
            return;
        }
        p = p->next;
    }
    printf("未找到该患者！\n");
}

void modifyPatientRecord(int patientId, int recordId, const char* newDiag, double newCost) {
    Patient* p = patientHead;
    while (p != NULL) {
        if (p->patientId == patientId) {
            VisitRecord* r = p->records;
            while (r != NULL) {
                if (r->recordId == recordId) {
                    p->totalCost = p->totalCost - r->cost + newCost;
                    strcpy(r->diagnosis, newDiag);
                    r->cost = newCost;
                    printf("病历修改成功！患者最新总费用：%.2f\n", p->totalCost);
                    return;
                }
                r = r->next;
            }
            printf("未找到该病历！\n");
            return;
        }
        p = p->next;
    }
    printf("未找到该患者！\n");
}

// ===================== 住院状态管理 =====================
void setPatientHospitalStatus(int patientId, bool isHosp, int bedId) {
    Patient* p = patientHead;
    while (p != NULL) {
        if (p->patientId == patientId) {
            p->isHospitalized = isHosp;
            p->bedId = bedId;
            Bed* b = bedHead;
            while (b != NULL) {
                if (b->bedId == bedId) {
                    b->isOccupied = isHosp;
                    b->patientId = isHosp ? patientId : -1;
                    break;
                }
                b = b->next;
            }
            printf("住院状态更新成功！%s | 床位ID：%d\n",
                   isHosp ? "已住院" : "已出院", bedId);
            return;
        }
        p = p->next;
    }
    printf("未找到该患者！\n");
}

void queryPatientHospitalStatus(int patientId) {
    Patient* p = patientHead;
    while (p != NULL) {
        if (p->patientId == patientId) {
            printf("患者【%s】(ID:%d)：%s | 床位ID：%d\n",
                   p->name, p->patientId,
                   p->isHospitalized ? "住院中" : "未住院",
                   p->bedId);
            return;
        }
        p = p->next;
    }
    printf("未找到该患者！\n");
}

// ===================== 挂号模块 A：创建挂号单 =====================
Registration* createRegistration(int rid, int pid, int did, char* dept, int type, int time) {
    Registration* reg = (Registration*)malloc(sizeof(Registration));
    reg->registerId = rid;
    reg->patientId = pid;
    reg->doctorId = did;
    strcpy(reg->department, dept);
    reg->registerType = type;
    reg->registerTime = time;
    reg->status = 0;
    reg->next = NULL;
    return reg;
}

void patientRegister(int registerId, int patientId, int doctorId, char* dept, int regType, int regTime) {
    Patient* p = patientHead;
    bool patientExist = false;
    while (p != NULL) {
        if (p->patientId == patientId) {
            patientExist = true;
            break;
        }
        p = p->next;
    }

    if (!patientExist) {
        printf("患者不存在，挂号失败！\n");
        return;
    }
    if (queryDoctorById(doctorId) == NULL) {
        printf("医生不存在，挂号失败！\n");
        return;
    }

    Registration* newReg = createRegistration(registerId, patientId, doctorId, dept, regType, regTime);
    if (regHead == NULL)
        regHead = newReg;
    else {
        Registration* t = regHead;
        while (t->next != NULL) t = t->next;
        t->next = newReg;
    }
    printf("挂号成功！挂号ID：%d，状态：候诊中\n", registerId);

  addIntoQueue(registerId, patientId, doctorId);

}

// ===================== 候诊队列模块 B =====================
void addIntoQueue(int registerId, int patientId, int doctorId) {
    static int qid = 1000;
    QueueNode* q = (QueueNode*)malloc(sizeof(QueueNode));
    q->queueId = qid++;
    q->registerId = registerId;
    q->patientId = patientId;
    q->doctorId = doctorId;
    q->status = 0;
    q->next = NULL;

    if (queueHead == NULL)
        queueHead = q;
    else {
        QueueNode* t = queueHead;
        while (t->next != NULL) t = t->next;
        t->next = q;
    }
}

// 查看全部候诊队列
void queryAllQueue() {
    if (queueHead == NULL) {
        printf("当前候诊队列为空！\n");
        return;
    }
    printf("=== 当前医生候诊队列 ===\n");
    QueueNode* t = queueHead;
    while (t != NULL) {
        char stat[10];
        switch (t->status) {
            case 0: strcpy(stat, "候诊中"); break;
            case 1: strcpy(stat, "已叫号"); break;
            case 2: strcpy(stat, "就诊中"); break;
            case 3: strcpy(stat, "已完成"); break;
        }
        printf("队列号：%d | 挂号ID：%d | 患者ID：%d | 医生ID：%d | 状态：%s\n",
               t->queueId, t->registerId, t->patientId, t->doctorId, stat);
        t = t->next;
    }
}

// 叫下一号
void callNextPatient() {
    if (queueHead == NULL) {
        printf("暂无候诊患者，无法叫号！\n");
        return;
    }
    QueueNode* first = queueHead;
    first->status = 2;

    Registration* r = regHead;
    while (r != NULL) {
        if (r->registerId == first->registerId) {
            r->status = 2;
            break;
        }
        r = r->next;
    }
    printf("叫号成功！患者进入就诊中\n");
}

// 完成接诊
void finishCurrentVisit() {
    if (queueHead == NULL) {
        printf("无正在就诊患者！\n");
        return;
    }
    QueueNode* current = queueHead;
    current->status = 3;

    Registration* r = regHead;
    while (r != NULL) {
        if (r->registerId == current->registerId) {
            r->status = 3;
            break;
        }
        r = r->next;
    }
    printf("本次就诊接诊完成！\n");
}

// ===================== 初始化测试数据 =====================
void initTestData() {
    Doctor* d1 = (Doctor*)malloc(sizeof(Doctor));
    d1->doctorId = 101; strcpy(d1->name, "张三"); strcpy(d1->department, "内科");
    strcpy(d1->title, "主任医师"); d1->patientCount = 0; d1->next = doctorHead; doctorHead = d1;

    Doctor* d2 = (Doctor*)malloc(sizeof(Doctor));
    d2->doctorId = 102; strcpy(d2->name, "李四"); strcpy(d2->department, "外科");
    strcpy(d2->title, "副主任医师"); d2->patientCount = 0; d2->next = doctorHead; doctorHead = d2;

    Patient* p1 = (Patient*)malloc(sizeof(Patient));
    p1->patientId = 201; strcpy(p1->name, "小明"); p1->age = 25; p1->gender = 'M';
    strcpy(p1->department, "内科"); p1->doctorId = 101; p1->isHospitalized = false;
    p1->bedId = -1; p1->totalCost = 0; p1->records = NULL; p1->next = patientHead; patientHead = p1;

    Bed* b1 = (Bed*)malloc(sizeof(Bed));
    b1->bedId = 301; strcpy(b1->wardType, "普通病房"); b1->isOccupied = false;
    b1->patientId = -1; b1->next = bedHead; bedHead = b1;
}

// ===================== 主函数 =====================
int B_entry(void) {
    initTestData();

    printf("===== 1. 查询所有科室 =====\n");
    queryAllDepartment();
    printf("\n===== 2. 查询内科医生 =====\n");
    queryDoctorByDept("内科");

    printf("\n===== 3. 患者小明现场挂号 =====\n");
    patientRegister(10001, 201, 101, "内科", 0, 1);
    patientRegister(10002, 201, 101, "内科", 1, 2);

    printf("\n===== 4. 查看当前候诊队列 =====\n");
    queryAllQueue();

    printf("\n===== 5. 叫号就诊 =====\n");
    callNextPatient();
    queryAllQueue();

    printf("\n===== 6. 结束本次就诊 =====\n");
    finishCurrentVisit();
    queryAllQueue();

    printf("\n===== 7. 患者就诊新增病历 =====\n");
    addPatientVisit(201, 1001, "感冒发烧", 150.5);
    queryPatientRecord(201);

    return 0;
}
