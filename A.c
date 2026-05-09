#include "common.h"
#include "utils.h"
#include <locale.h>


/* ===== 为了让 A 模块单独成文件可运行，补充最小医生/药品/床位辅助实现 ===== */
static Doctor *doctor_create(int doctorId, const char *name, const char *department, const char *title) {
    Doctor *d = (Doctor *)malloc(sizeof(Doctor));
    if (!d) return NULL;
    d->doctorId = doctorId;
    strncpy(d->name, name, NAME_LEN - 1);
    d->name[NAME_LEN - 1] = '\0';
    strncpy(d->department, department, DEPT_LEN - 1);
    d->department[DEPT_LEN - 1] = '\0';
    strncpy(d->title, title, TITLE_LEN - 1);
    d->title[TITLE_LEN - 1] = '\0';
    d->patientCount = 0;
    d->next = NULL;
    return d;
}

static int doctor_add(Doctor **head, Doctor *newDoctor) {
    if (!head || !newDoctor) return 0;
    if (!*head) {
        *head = newDoctor;
        return 1;
    }
    Doctor *cur = *head;
    while (cur->next) cur = cur->next;
    cur->next = newDoctor;
    return 1;
}

static Doctor *doctor_find_by_id(Doctor *head, int doctorId) {
    while (head) {
        if (head->doctorId == doctorId) return head;
        head = head->next;
    }
    return NULL;
}

static void doctor_display_all(Doctor *head) {
    ui_print_sub_title("医生列表");
    while (head) {
        printf("%d | %s | %s | %s | 已接诊:%d\n",
               head->doctorId, head->name, head->department, head->title, head->patientCount);
        head = head->next;
    }
}

static void doctor_free_all(Doctor *head) {
    while (head) {
        Doctor *tmp = head;
        head = head->next;
        free(tmp);
    }
}

static Medicine *medicine_create(int medicineId, const char *name, float price, int stock, int warningLine) {
    Medicine *m = (Medicine *)malloc(sizeof(Medicine));
    if (!m) return NULL;
    m->medicineId = medicineId;
    strncpy(m->name, name, MED_NAME_LEN - 1);
    m->name[MED_NAME_LEN - 1] = '\0';
    m->price = price;
    m->stock = stock;
    m->warningLine = warningLine;
    m->next = NULL;
    return m;
}

static int medicine_add(Medicine **head, Medicine *newMedicine) {
    if (!head || !newMedicine) return 0;
    if (!*head) {
        *head = newMedicine;
        return 1;
    }
    Medicine *cur = *head;
    while (cur->next) cur = cur->next;
    cur->next = newMedicine;
    return 1;
}

static void medicine_free_all(Medicine *head) {
    while (head) {
        Medicine *tmp = head;
        head = head->next;
        free(tmp);
    }
}

static Bed *bed_create(int bedId, const char *wardType) {
    Bed *b = (Bed *)malloc(sizeof(Bed));
    if (!b) return NULL;
    b->bedId = bedId;
    strncpy(b->wardType, wardType, WARD_TYPE_LEN - 1);
    b->wardType[WARD_TYPE_LEN - 1] = '\0';
    b->isOccupied = 0;
    b->patientId = -1;
    b->next = NULL;
    return b;
}

static int bed_add(Bed **head, Bed *newBed) {
    if (!head || !newBed) return 0;
    if (!*head) {
        *head = newBed;
        return 1;
    }
    Bed *cur = *head;
    while (cur->next) cur = cur->next;
    cur->next = newBed;
    return 1;
}

static void bed_free_all(Bed *head) {
    while (head) {
        Bed *tmp = head;
        head = head->next;
        free(tmp);
    }
}

/* 前向声明，避免单文件内前后顺序导致的编译问题 */
void patient_display_one(const Patient *patient);

/************************ A：patient.c ************************/

Patient *patient_create(int patientId, const char *name, int age, const char *gender, const char *department) {
    Patient *p = (Patient *)malloc(sizeof(Patient));
    if (!p) return NULL;
    p->patientId = patientId;
    strncpy(p->name, name, NAME_LEN - 1);
    p->name[NAME_LEN - 1] = '\0';
    p->age = age;
    strncpy(p->gender, gender, GENDER_LEN - 1);
    p->gender[GENDER_LEN - 1] = '\0';
    strncpy(p->department, department, DEPT_LEN - 1);
    p->department[DEPT_LEN - 1] = '\0';
    p->doctorId = -1;
    p->isHospitalized = 0;
    p->bedId = -1;
    p->totalCost = 0.0f;
    p->records = NULL;
    p->next = NULL;
    return p;
}

int patient_add(Patient **head, Patient *newPatient) {
    if (!head || !newPatient) return 0;
    if (*head == NULL) {
        *head = newPatient;
        return 1;
    }
    Patient *cur = *head;
    while (cur->next) cur = cur->next;
    cur->next = newPatient;
    return 1;
}

Patient *patient_find_by_id(Patient *head, int patientId) {
    while (head) {
        if (head->patientId == patientId) return head;
        head = head->next;
    }
    return NULL;
}

void patient_find_by_name(Patient *head, const char *name) {
    int found = 0;
    while (head) {
        if (strcmp(head->name, name) == 0) {
            patient_display_one(head);
            ui_print_line();
            found = 1;
        }
        head = head->next;
    }
    if (!found) printf("未找到对应信息！\n");
}

int patient_update(Patient *head, int patientId) {
    Patient *p = patient_find_by_id(head, patientId);
    if (!p) return 0;

    char name[NAME_LEN], gender[GENDER_LEN], dept[DEPT_LEN];
    int age;
    utils_input_string("请输入姓名：", name, sizeof(name));
    age = utils_input_int("请输入年龄：", 0, 150);
    utils_input_string("请输入性别：", gender, sizeof(gender));
    utils_input_string("请输入科室：", dept, sizeof(dept));

    strncpy(p->name, name, NAME_LEN - 1);
    strncpy(p->gender, gender, GENDER_LEN - 1);
    strncpy(p->department, dept, DEPT_LEN - 1);
    p->name[NAME_LEN - 1] = '\0';
    p->gender[GENDER_LEN - 1] = '\0';
    p->department[DEPT_LEN - 1] = '\0';
    p->age = age;
    return 1;
}

static void free_records(VisitRecord *head) {
    VisitRecord *tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

int patient_delete(Patient **head, int patientId) {
    if (!head || !*head) return 0;
    Patient *cur = *head, *prev = NULL;
    while (cur) {
        if (cur->patientId == patientId) {
            if (prev) prev->next = cur->next;
            else *head = cur->next;
            free_records(cur->records);
            free(cur);
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }
    return 0;
}

void patient_display_one(const Patient *patient) {
    if (!patient) return;
    printf("患者编号：%d\n", patient->patientId);
    printf("姓名：%s\n", patient->name);
    printf("年龄：%d\n", patient->age);
    printf("性别：%s\n", patient->gender);
    printf("科室：%s\n", patient->department);
    printf("医生编号：%d\n", patient->doctorId);
    printf("是否住院：%s\n", patient->isHospitalized ? "是" : "否");
    printf("床位编号：%d\n", patient->bedId);
    printf("总费用：%.2f\n", patient->totalCost);
}

void patient_display_all(Patient *head) {
    ui_print_sub_title("患者列表");
    if (!head) {
        printf("暂无患者信息。\n");
        return;
    }
    printf("%-8s %-10s %-6s %-6s %-8s %-8s %-8s %-8s\n",
           "编号", "姓名", "年龄", "性别", "科室", "医生", "住院", "费用");
    ui_print_line();
    while (head) {
        printf("%-8d %-10s %-6d %-6s %-8s %-8d %-8s %-8.2f\n",
               head->patientId, head->name, head->age, head->gender,
               head->department, head->doctorId,
               head->isHospitalized ? "是" : "否", head->totalCost);
        head = head->next;
    }
}

VisitRecord *record_create(int recordId, const char *diagnosis, float cost) {
    VisitRecord *r = (VisitRecord *)malloc(sizeof(VisitRecord));
    if (!r) return NULL;
    r->recordId = recordId;
    strncpy(r->diagnosis, diagnosis, DIAGNOSIS_LEN - 1);
    r->diagnosis[DIAGNOSIS_LEN - 1] = '\0';
    r->cost = cost;
    r->next = NULL;
    return r;
}

int patient_add_record(Patient *patient, VisitRecord *record) {
    if (!patient || !record) return 0;
    if (!patient->records) {
        patient->records = record;
    } else {
        VisitRecord *cur = patient->records;
        while (cur->next) cur = cur->next;
        cur->next = record;
    }
    return 1;
}

void patient_show_records(const Patient *patient) {
    if (!patient) return;
    ui_print_sub_title("就诊记录");
    if (!patient->records) {
        printf("暂无就诊记录。\n");
        return;
    }
    VisitRecord *cur = patient->records;
    while (cur) {
        printf("记录编号：%d | 诊断：%s | 本次费用：%.2f\n", cur->recordId, cur->diagnosis, cur->cost);
        cur = cur->next;
    }
}

void patient_register(Patient **head, int *nextPatientId) {
    char name[NAME_LEN], gender[GENDER_LEN], dept[DEPT_LEN];
    int age;
    utils_input_string("请输入姓名：", name, sizeof(name));
    age = utils_input_int("请输入年龄：", 0, 150);
    utils_input_string("请输入性别：", gender, sizeof(gender));
    utils_input_string("请输入科室：", dept, sizeof(dept));

    Patient *p = patient_create((*nextPatientId)++, name, age, gender, dept);
    if (!p || !patient_add(head, p)) {
        printf("操作失败！\n");
        free(p);
        return;
    }
    printf("新增患者成功！患者编号为：%d\n", p->patientId);
}

static const char *registration_type_text(int registerType) {
    return registerType == REGISTER_BOOK ? "预约挂号" : "现场挂号";
}

static const char *registration_status_text(int status) {
    switch (status) {
        case STATUS_WAIT: return "候诊中";
        case STATUS_CALL: return "已叫号";
        case STATUS_TREATING: return "就诊中";
        case STATUS_FINISH: return "已完成";
        default: return "未知状态";
    }
}

Registration *registration_create(int registerId, int patientId, int doctorId, const char *department,
                                  int registerType, int registerTime) {
    Registration *r = (Registration *)malloc(sizeof(Registration));
    if (!r) return NULL;
    r->registerId = registerId;
    r->patientId = patientId;
    r->doctorId = doctorId;
    strncpy(r->department, department, DEPT_LEN - 1);
    r->department[DEPT_LEN - 1] = '\0';
    r->registerType = registerType;
    r->registerTime = registerTime;
    r->status = STATUS_WAIT;
    r->next = NULL;
    return r;
}

int registration_add(Registration **head, Registration *newRegistration) {
    if (!head || !newRegistration) return 0;
    if (!*head) {
        *head = newRegistration;
        return 1;
    }
    Registration *cur = *head;
    while (cur->next) cur = cur->next;
    cur->next = newRegistration;
    return 1;
}

Registration *registration_find_by_id(Registration *head, int registerId) {
    while (head) {
        if (head->registerId == registerId) return head;
        head = head->next;
    }
    return NULL;
}

void registration_display_one(const Registration *registration) {
    if (!registration) return;
    printf("挂号编号：%d\n", registration->registerId);
    printf("患者编号：%d\n", registration->patientId);
    printf("医生编号：%d\n", registration->doctorId);
    printf("科室：%s\n", registration->department);
    printf("挂号方式：%s\n", registration_type_text(registration->registerType));
    printf("挂号顺序号：%d\n", registration->registerTime);
    printf("当前状态：%s\n", registration_status_text(registration->status));
}

void registration_display_all(Registration *head) {
    ui_print_sub_title("挂号记录列表");
    if (!head) {
        printf("暂无挂号记录。\n");
        return;
    }
    printf("%-8s %-8s %-8s %-8s %-10s %-10s %-10s\n",
           "挂号号", "患者", "医生", "科室", "挂号方式", "顺序号", "状态");
    ui_print_line();
    while (head) {
        printf("%-8d %-8d %-8d %-8s %-10s %-10d %-10s\n",
               head->registerId, head->patientId, head->doctorId, head->department,
               registration_type_text(head->registerType), head->registerTime,
               registration_status_text(head->status));
        head = head->next;
    }
}

void registration_display_by_patient(Registration *head, int patientId) {
    int found = 0;
    while (head) {
        if (head->patientId == patientId) {
            registration_display_one(head);
            ui_print_line();
            found = 1;
        }
        head = head->next;
    }
    if (!found) printf("该患者暂无挂号记录。\n");
}

static int patient_register_common(Patient *pHead, Doctor *dHead, Registration **regHead,
                                   int *nextRegisterId, int *nextRegisterTime, int registerType) {
    if (!pHead) {
        printf("当前无患者信息，请先新增患者。\n");
        return 0;
    }
    if (!dHead) {
        printf("当前无医生信息，无法挂号。\n");
        return 0;
    }

    patient_display_all(pHead);
    int patientId = utils_input_int("请输入患者编号：", 1, 999999);
    Patient *patient = patient_find_by_id(pHead, patientId);
    if (!patient) {
        printf("未找到对应患者信息！\n");
        return 0;
    }

    printf("可选医生如下：\n");
    doctor_display_all(dHead);
    int doctorId = utils_input_int("请输入医生编号：", 1, 999999);
    Doctor *doctor = doctor_find_by_id(dHead, doctorId);
    if (!doctor) {
        printf("未找到对应医生信息！\n");
        return 0;
    }

    if (strcmp(patient->department, doctor->department) != 0) {
        printf("挂号失败：患者所属科室与医生所属科室不一致。\n");
        return 0;
    }

    Registration *reg = registration_create((*nextRegisterId)++, patient->patientId,
                                            doctor->doctorId, patient->department,
                                            registerType, (*nextRegisterTime)++);
    if (!reg || !registration_add(regHead, reg)) {
        printf("挂号失败！\n");
        free(reg);
        return 0;
    }

    patient->doctorId = doctor->doctorId;
    printf("%s成功！挂号编号：%d，当前状态：%s\n",
           registration_type_text(registerType), reg->registerId,
           registration_status_text(reg->status));
    return 1;
}

void patient_register_appointment(Patient *pHead, Doctor *dHead, Registration **regHead,
                                  int *nextRegisterId, int *nextRegisterTime) {
    patient_register_common(pHead, dHead, regHead, nextRegisterId, nextRegisterTime, REGISTER_BOOK);
}

void patient_register_onsite(Patient *pHead, Doctor *dHead, Registration **regHead,
                             int *nextRegisterId, int *nextRegisterTime) {
    patient_register_common(pHead, dHead, regHead, nextRegisterId, nextRegisterTime, REGISTER_ON_SITE);
}

void registration_free_all(Registration *head) {
    Registration *tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

static void registration_query_menu(Registration *regHead) {
    int choice;
    while (1) {
        ui_print_sub_title("挂号记录查询");
        printf("1. 按挂号编号查询\n");
        printf("2. 按患者编号查询\n");
        printf("3. 显示全部挂号记录\n");
        printf("0. 返回上一级\n");
        ui_print_line();
        choice = utils_input_int("请输入选项：", 0, 3);
        if (choice == 0) break;

        if (choice == 1) {
            int registerId = utils_input_int("请输入挂号编号：", 1, 999999);
            Registration *reg = registration_find_by_id(regHead, registerId);
            if (!reg) printf("未找到对应挂号记录！\n");
            else registration_display_one(reg);
        } else if (choice == 2) {
            int patientId = utils_input_int("请输入患者编号：", 1, 999999);
            registration_display_by_patient(regHead, patientId);
        } else if (choice == 3) {
            registration_display_all(regHead);
        }
        utils_wait_enter();
    }
}

static void swap_patient_nodes(Patient *a, Patient *b) {
    int tempId = a->patientId; a->patientId = b->patientId; b->patientId = tempId;
    char tempName[NAME_LEN]; strcpy(tempName, a->name); strcpy(a->name, b->name); strcpy(b->name, tempName);
    int tempAge = a->age; a->age = b->age; b->age = tempAge;
    char tempGender[GENDER_LEN]; strcpy(tempGender, a->gender); strcpy(a->gender, b->gender); strcpy(b->gender, tempGender);
    char tempDept[DEPT_LEN]; strcpy(tempDept, a->department); strcpy(a->department, b->department); strcpy(b->department, tempDept);
    int tempDoctorId = a->doctorId; a->doctorId = b->doctorId; b->doctorId = tempDoctorId;
    int tempHosp = a->isHospitalized; a->isHospitalized = b->isHospitalized; b->isHospitalized = tempHosp;
    int tempBedId = a->bedId; a->bedId = b->bedId; b->bedId = tempBedId;
    float tempCost = a->totalCost; a->totalCost = b->totalCost; b->totalCost = tempCost;
    VisitRecord *tempRecords = a->records; a->records = b->records; b->records = tempRecords;
}

void patient_sort_by_age(Patient **head) {
    if (!head || !*head) return;
    int swapped;
    Patient *ptr;
    Patient *lptr = NULL;
    do {
        swapped = 0;
        ptr = *head;
        while (ptr->next != lptr) {
            if (ptr->age > ptr->next->age) {
                swap_patient_nodes(ptr, ptr->next);
                swapped = 1;
            }
            ptr = ptr->next;
        }
        lptr = ptr;
    } while (swapped);
}

void patient_free_all(Patient *head) {
    Patient *tmp;
    while (head) {
        free_records(head->records);
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

void patient_menu(Patient **pHead, Doctor *dHead, Registration **regHead,
                  int *nextPatientId, int *nextRegisterId,
                  int *nextRegisterTime, int *nextRecordId) {
    int choice;
    while (1) {
        ui_print_sub_title("患者与挂号管理");
        printf("1. 新增患者\n");
        printf("2. 预约挂号\n");
        printf("3. 现场挂号\n");
        printf("4. 查询患者信息\n");
        printf("5. 修改患者信息\n");
        printf("6. 删除患者信息\n");
        printf("7. 添加就诊记录\n");
        printf("8. 显示所有患者\n");
        printf("9. 查询挂号记录\n");
        printf("10. 按年龄排序\n");
        printf("0. 返回主菜单\n");
        ui_print_line();
        choice = utils_input_int("请输入选项：", 0, 10);
        if (choice == 0) break;

        int id;
        Patient *p;
        char name[NAME_LEN];
        char diagnosis[DIAGNOSIS_LEN];
        float cost;
        switch (choice) {
            case 1:
                patient_register(pHead, nextPatientId);
                break;
            case 2:
                patient_register_appointment(*pHead, dHead, regHead, nextRegisterId, nextRegisterTime);
                break;
            case 3:
                patient_register_onsite(*pHead, dHead, regHead, nextRegisterId, nextRegisterTime);
                break;
            case 4:
                id = utils_input_int("按编号查询请输入患者编号（若按姓名查询请输入 0）：", 0, 999999);
                if (id == 0) {
                    utils_input_string("请输入姓名：", name, sizeof(name));
                    patient_find_by_name(*pHead, name);
                } else {
                    p = patient_find_by_id(*pHead, id);
                    if (!p) printf("未找到对应信息！\n");
                    else {
                        patient_display_one(p);
                        patient_show_records(p);
                    }
                }
                break;
            case 5:
                id = utils_input_int("请输入患者编号：", 1, 999999);
                printf(patient_update(*pHead, id) ? "修改成功！\n" : "未找到对应信息！\n");
                break;
            case 6:
                id = utils_input_int("请输入患者编号：", 1, 999999);
                printf(patient_delete(pHead, id) ? "删除成功！\n" : "未找到对应信息！\n");
                break;
            case 7:
                id = utils_input_int("请输入患者编号：", 1, 999999);
                p = patient_find_by_id(*pHead, id);
                if (!p) {
                    printf("未找到对应信息！\n");
                    break;
                }
                utils_input_string("请输入诊断信息：", diagnosis, sizeof(diagnosis));
                cost = utils_input_float("请输入本次费用：", 0.0f, 1000000.0f);
                if (patient_add_record(p, record_create((*nextRecordId)++, diagnosis, cost))) {
                    p->totalCost += cost;
                    printf("添加成功！\n");
                } else {
                    printf("操作失败！\n");
                }
                break;
            case 8:
                patient_display_all(*pHead);
                break;
            case 9:
                registration_query_menu(*regHead);
                break;
            case 10:
                patient_sort_by_age(pHead);
                printf("排序成功！\n");
                patient_display_all(*pHead);
                break;
        }
        utils_wait_enter();
    }
}

/************************ A：init.c ************************/

void init_patients(Patient **pHead, int *nextPatientId, int *nextRecordId) {
    Patient *p;
    p = patient_create((*nextPatientId)++, "张三", 20, "男", "内科");
    patient_add_record(p, record_create((*nextRecordId)++, "普通感冒", 35.0f));
    p->totalCost += 35.0f;
    patient_add(pHead, p);

    p = patient_create((*nextPatientId)++, "李四", 31, "男", "外科");
    patient_add(pHead, p);

    p = patient_create((*nextPatientId)++, "王五", 8, "女", "儿科");
    patient_add(pHead, p);

    p = patient_create((*nextPatientId)++, "赵六", 45, "男", "骨科");
    patient_add(pHead, p);

    p = patient_create((*nextPatientId)++, "小张", 28, "女", "内科");
    patient_add(pHead, p);
}

void init_doctors(Doctor **dHead, int *nextDoctorId) {
    doctor_add(dHead, doctor_create((*nextDoctorId)++, "陈医生", "内科", "主治医师"));
    doctor_add(dHead, doctor_create((*nextDoctorId)++, "刘医生", "内科", "副主任医师"));
    doctor_add(dHead, doctor_create((*nextDoctorId)++, "孙医生", "外科", "主治医师"));
    doctor_add(dHead, doctor_create((*nextDoctorId)++, "周医生", "外科", "主任医师"));
    doctor_add(dHead, doctor_create((*nextDoctorId)++, "吴医生", "儿科", "主治医师"));
    doctor_add(dHead, doctor_create((*nextDoctorId)++, "郑医生", "儿科", "副主任医师"));
    doctor_add(dHead, doctor_create((*nextDoctorId)++, "王医生", "骨科", "主治医师"));
    doctor_add(dHead, doctor_create((*nextDoctorId)++, "冯医生", "骨科", "主任医师"));
}

void init_medicines(Medicine **mHead, int *nextMedicineId) {
    medicine_add(mHead, medicine_create((*nextMedicineId)++, "阿莫西林", 19.0f, 60, 10));
    medicine_add(mHead, medicine_create((*nextMedicineId)++, "布洛芬", 12.5f, 50, 10));
    medicine_add(mHead, medicine_create((*nextMedicineId)++, "头孢克肟", 28.0f, 40, 8));
    medicine_add(mHead, medicine_create((*nextMedicineId)++, "维生素C", 8.0f, 100, 20));
    medicine_add(mHead, medicine_create((*nextMedicineId)++, "止咳糖浆", 16.0f, 25, 5));
    medicine_add(mHead, medicine_create((*nextMedicineId)++, "云南白药", 22.0f, 30, 6));
    medicine_add(mHead, medicine_create((*nextMedicineId)++, "创可贴", 5.0f, 80, 15));
    medicine_add(mHead, medicine_create((*nextMedicineId)++, "葡萄糖", 14.0f, 35, 8));
    medicine_add(mHead, medicine_create((*nextMedicineId)++, "奥美拉唑", 26.0f, 24, 6));
    medicine_add(mHead, medicine_create((*nextMedicineId)++, "蒙脱石散", 18.0f, 18, 5));
    medicine_add(mHead, medicine_create((*nextMedicineId)++, "氯雷他定", 21.0f, 22, 5));
    medicine_add(mHead, medicine_create((*nextMedicineId)++, "碘伏", 11.0f, 32, 6));
    medicine_add(mHead, medicine_create((*nextMedicineId)++, "感冒灵", 13.0f, 45, 10));
    medicine_add(mHead, medicine_create((*nextMedicineId)++, "双黄连", 17.0f, 28, 6));
    medicine_add(mHead, medicine_create((*nextMedicineId)++, "阿司匹林", 9.5f, 55, 12));
}

void init_beds(Bed **bHead, int *nextBedId) {
    int i;
    for (i = 0; i < 10; ++i) bed_add(bHead, bed_create((*nextBedId)++, "普通病房"));
    for (i = 0; i < 6; ++i) bed_add(bHead, bed_create((*nextBedId)++, "单人病房"));
    for (i = 0; i < 4; ++i) bed_add(bHead, bed_create((*nextBedId)++, "监护病房"));
}

void init_system(Patient **pHead, Doctor **dHead, Medicine **mHead, Bed **bHead,
                 int *nextPatientId, int *nextDoctorId, int *nextMedicineId,
                 int *nextPrescriptionId, int *nextBedId, int *nextRecordId,
                 int *nextRegisterId, int *nextQueueId, int *nextRegisterTime) {
    *nextPatientId = PATIENT_ID_START;
    *nextDoctorId = DOCTOR_ID_START;
    *nextMedicineId = MEDICINE_ID_START;
    *nextPrescriptionId = PRESCRIPTION_ID_START;
    *nextBedId = BED_ID_START;
    *nextRecordId = RECORD_ID_START;
    *nextRegisterId = REGISTER_ID_START;
    *nextQueueId = QUEUE_ID_START;
    *nextRegisterTime = REGISTER_TIME_START;

    *pHead = NULL;
    *dHead = NULL;
    *mHead = NULL;
    *bHead = NULL;

    init_patients(pHead, nextPatientId, nextRecordId);
    init_doctors(dHead, nextDoctorId);
    init_medicines(mHead, nextMedicineId);
    init_beds(bHead, nextBedId);
}

/************************ A：main.c ************************/



int A_entry(void) {
    setlocale(LC_ALL, "");
    Patient *pHead = NULL;
    Doctor *dHead = NULL;
    Medicine *mHead = NULL;
    Bed *bHead = NULL;
    Registration *regHead = NULL;
    int nextPatientId, nextDoctorId, nextMedicineId, nextPrescriptionId;
    int nextBedId, nextRecordId, nextRegisterId, nextQueueId, nextRegisterTime;

    init_system(&pHead, &dHead, &mHead, &bHead,
                &nextPatientId, &nextDoctorId, &nextMedicineId,
                &nextPrescriptionId, &nextBedId, &nextRecordId,
                &nextRegisterId, &nextQueueId, &nextRegisterTime);

    patient_menu(&pHead, dHead, &regHead,
                 &nextPatientId, &nextRegisterId,
                 &nextRegisterTime, &nextRecordId);

    registration_free_all(regHead);
    bed_free_all(bHead);
    medicine_free_all(mHead);
    doctor_free_all(dHead);
    patient_free_all(pHead);
    return 0;
}
