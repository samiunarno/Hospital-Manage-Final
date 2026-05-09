#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LENGTH 100
#define MAX_DEPARTMENTS 10
#define MAX_WARDS_PER_DEPT 10
#define MAX_BEDS_PER_WARD 20
#define SUCCESS 1
#define FAILURE 0
#define TRUE 1
#define FALSE 0

typedef enum {
    STATUS_AVAILABLE = 0,
    STATUS_OCCUPIED = 1
} BedStatus;

typedef enum {
    REPORT_CONSOLE = 1,
    REPORT_FILE = 2,
    REPORT_BOTH = 3
} ReportType;

typedef struct Bed {
    int bedNumber;
    BedStatus status;
    int patientId;
    char patientName[100];
    struct Bed* next;
} Bed;

typedef struct Ward {
    int wardNumber;
    char wardName[MAX_NAME_LENGTH];
    int totalBeds;
    int occupiedBeds;
    Bed* bedList;
    struct Ward* next;
} Ward;

typedef struct Department {
    int departmentId;
    char departmentName[MAX_NAME_LENGTH];
    int totalWards;
    int totalBeds;
    int totalPatients;
    Ward* wardList;
    struct Department* next;
} Department;

typedef struct HospitalStatistics {
    int totalDepartments;
    int totalWards;
    int totalBeds;
    int totalPatients;
    float overallOccupancyRate;
    float departmentOccupancyRates[MAX_DEPARTMENTS];
    char departmentNames[MAX_DEPARTMENTS][MAX_NAME_LENGTH];
    int departmentCount;
} HospitalStatistics;

typedef struct PatientInfo {
    int patientId;
    char patientName[MAX_NAME_LENGTH];
    int departmentId;
    int wardNumber;
    int bedNumber;
} PatientInfo;

// Function declarations
void initializeBed(Bed* bed, int bedNumber);
void addBedToWard(Ward* ward, Bed* bed);
void displayWardBeds(const Ward* ward);
void updateDepartmentStats(Department* department);
int getValidatedIntInput(const char* prompt, int min, int max);
void freeHospitalSystem(Department** hospital);
void printMemoryReport(void);

// Bed functions
Bed* createBed(int bedNumber) {
    Bed* bed = (Bed*)malloc(sizeof(Bed));
    if (bed == NULL) {
        printf("床位内存分配失败\n");
        return NULL;
    }
    initializeBed(bed, bedNumber);
    return bed;
}

void initializeBed(Bed* bed, int bedNumber) {
    bed->bedNumber = bedNumber;
    bed->status = STATUS_AVAILABLE;
    bed->patientId = -1;
    strcpy(bed->patientName, "");
    bed->next = NULL;
}

void occupyBed(Bed* bed, int patientId, const char* patientName) {
    if (bed == NULL) return;
    bed->status = STATUS_OCCUPIED;
    bed->patientId = patientId;
    strcpy(bed->patientName, patientName);
}

void vacateBed(Bed* bed) {
    if (bed == NULL) return;
    bed->status = STATUS_AVAILABLE;
    bed->patientId = -1;
    strcpy(bed->patientName, "");
}

int isBedAvailable(const Bed* bed) {
    return bed != NULL && bed->status == STATUS_AVAILABLE;
}

void displayBed(const Bed* bed) {
    if (bed == NULL) return;
    
    if (bed->status == STATUS_OCCUPIED) {
        printf("  床位 %2d: [已占用] 病人: %s (编号: %d)\n", 
               bed->bedNumber, bed->patientName, bed->patientId);
    } else {
        printf("  床位 %2d: [空闲]\n", bed->bedNumber);
    }
}

void freeBed(Bed* bed) {
    if (bed != NULL) {
        free(bed);
    }
}

// Ward functions
Ward* createWard(int wardNumber, const char* wardName, int totalBeds) {
    Ward* ward = (Ward*)malloc(sizeof(Ward));
    if (ward == NULL) {
        printf("病房内存分配失败\n");
        return NULL;
    }
    
    ward->wardNumber = wardNumber;
    strcpy(ward->wardName, wardName);
    ward->totalBeds = totalBeds;
    ward->occupiedBeds = 0;
    ward->bedList = NULL;
    ward->next = NULL;
    
    for (int i = 1; i <= totalBeds; i++) {
        Bed* bed = createBed(i);
        addBedToWard(ward, bed);
    }
    
    return ward;
}

void addBedToWard(Ward* ward, Bed* bed) {
    if (ward == NULL || bed == NULL) return;
    
    if (ward->bedList == NULL) {
        ward->bedList = bed;
    } else {
        Bed* current = ward->bedList;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = bed;
    }
}

Bed* findBedInWard(const Ward* ward, int bedNumber) {
    if (ward == NULL) return NULL;
    
    Bed* current = ward->bedList;
    while (current != NULL) {
        if (current->bedNumber == bedNumber) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int getAvailableBedsInWard(const Ward* ward) {
    if (ward == NULL) return 0;
    return ward->totalBeds - ward->occupiedBeds;
}

float getWardOccupancyRate(const Ward* ward) {
    if (ward == NULL || ward->totalBeds == 0) return 0.0f;
    return (float)ward->occupiedBeds / ward->totalBeds * 100;
}

void displayWardBeds(const Ward* ward) {
    if (ward == NULL) return;
    
    Bed* current = ward->bedList;
    while (current != NULL) {
        displayBed(current);
        current = current->next;
    }
}

void displayWard(const Ward* ward) {
    if (ward == NULL) return;
    
    printf("\n============================================================\n");
    printf("病房 %d: %s\n", ward->wardNumber, ward->wardName);
    printf("入住率: %d/%d 床位 (%.1f%%)\n", 
           ward->occupiedBeds, ward->totalBeds, getWardOccupancyRate(ward));
    printf("------------------------------------------------------------\n");
    displayWardBeds(ward);
    printf("============================================================\n");
}

void freeWard(Ward* ward) {
    if (ward == NULL) return;
    
    Bed* current = ward->bedList;
    while (current != NULL) {
        Bed* next = current->next;
        freeBed(current);
        current = next;
    }
    
    free(ward);
}

// Department functions
Department* createDepartment(int departmentId, const char* departmentName) {
    Department* department = (Department*)malloc(sizeof(Department));
    if (department == NULL) {
        printf("科室内存分配失败\n");
        return NULL;
    }
    
    department->departmentId = departmentId;
    strcpy(department->departmentName, departmentName);
    department->totalWards = 0;
    department->totalBeds = 0;
    department->totalPatients = 0;
    department->wardList = NULL;
    department->next = NULL;
    
    return department;
}

void addWardToDepartment(Department* department, Ward* ward) {
    if (department == NULL || ward == NULL) return;
    
    if (department->wardList == NULL) {
        department->wardList = ward;
    } else {
        Ward* current = department->wardList;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = ward;
    }
    
    department->totalWards++;
    department->totalBeds += ward->totalBeds;
    updateDepartmentStats(department);
}

Ward* findWardInDepartment(const Department* department, int wardNumber) {
    if (department == NULL) return NULL;
    
    Ward* current = department->wardList;
    while (current != NULL) {
        if (current->wardNumber == wardNumber) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void updateDepartmentStats(Department* department) {
    if (department == NULL) return;
    
    department->totalPatients = 0;
    Ward* current = department->wardList;
    
    while (current != NULL) {
        department->totalPatients += current->occupiedBeds;
        current = current->next;
    }
}

void displayDepartment(const Department* department) {
    if (department == NULL) return;
    
    printf("\n============================================================\n");
    printf("科室: %s (编号: %d)\n", department->departmentName, department->departmentId);
    printf("统计: %d 位病人 | %d 张床位 | %d 个病房\n", 
           department->totalPatients, department->totalBeds, department->totalWards);
    printf("============================================================\n");
    
    Ward* current = department->wardList;
    while (current != NULL) {
        displayWard(current);
        current = current->next;
    }
}

void freeDepartment(Department* department) {
    if (department == NULL) return;
    
    Ward* current = department->wardList;
    while (current != NULL) {
        Ward* next = current->next;
        freeWard(current);
        current = next;
    }
    
    free(department);
}

// Statistics functions
HospitalStatistics collectStatistics(Department* hospital) {
    HospitalStatistics stats;
    memset(&stats, 0, sizeof(stats));
    
    Department* dept = hospital;
    int deptIndex = 0;
    
    while (dept != NULL && deptIndex < MAX_DEPARTMENTS) {
        stats.totalDepartments++;
        stats.totalWards += dept->totalWards;
        stats.totalBeds += dept->totalBeds;
        stats.totalPatients += dept->totalPatients;
        
        if (dept->totalBeds > 0) {
            stats.departmentOccupancyRates[deptIndex] = 
                (float)dept->totalPatients / dept->totalBeds * 100;
        }
        strcpy(stats.departmentNames[deptIndex], dept->departmentName);
        deptIndex++;
        
        dept = dept->next;
    }
    
    stats.departmentCount = deptIndex;
    
    if (stats.totalBeds > 0) {
        stats.overallOccupancyRate = (float)stats.totalPatients / stats.totalBeds * 100;
    }
    
    return stats;
}

void displayStatisticsReport(const HospitalStatistics* stats) {
    printf("\n");
    printf("============================================================\n");
    printf("                    医院统计报告\n");
    printf("============================================================\n");
    
    time_t t;
    time(&t);
    printf("报告日期: %s", ctime(&t));
    printf("============================================================\n");
    
    printf("医院总体情况\n");
    printf("  总科室数:       %d\n", stats->totalDepartments);
    printf("  总病房数:       %d\n", stats->totalWards);
    printf("  总床位数:       %d\n", stats->totalBeds);
    printf("  总病人数:       %d\n", stats->totalPatients);
    printf("  全院入住率:     %.1f%%\n", stats->overallOccupancyRate);
    
    printf("============================================================\n");
    printf("各科室详情\n");
    
    for (int i = 0; i < stats->departmentCount; i++) {
        printf("  %-15s %.1f%% 入住率\n", 
               stats->departmentNames[i], stats->departmentOccupancyRates[i]);
    }
    
    printf("============================================================\n");
}

void exportStatisticsToFile(const HospitalStatistics* stats, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("错误：无法创建报告文件\n");
        return;
    }
    
    fprintf(file, "============================================================\n");
    fprintf(file, "医院统计报告\n");
    fprintf(file, "============================================================\n\n");
    
    time_t t;
    time(&t);
    fprintf(file, "报告日期: %s\n\n", ctime(&t));
    
    fprintf(file, "医院总体情况\n");
    fprintf(file, "------------------------------------------------------------\n");
    fprintf(file, "总科室数:       %d\n", stats->totalDepartments);
    fprintf(file, "总病房数:       %d\n", stats->totalWards);
    fprintf(file, "总床位数:       %d\n", stats->totalBeds);
    fprintf(file, "总病人数:       %d\n", stats->totalPatients);
    fprintf(file, "全院入住率:     %.1f%%\n\n", stats->overallOccupancyRate);
    
    fprintf(file, "各科室详情\n");
    fprintf(file, "------------------------------------------------------------\n");
    for (int i = 0; i < stats->departmentCount; i++) {
        fprintf(file, "%s: %.1f%% 入住率\n", 
                stats->departmentNames[i], stats->departmentOccupancyRates[i]);
    }
    
    fclose(file);
    printf("\n✓ 报告已导出到 '%s'\n", filename);
}

void generateStatisticsReport(Department* hospital, ReportType type) {
    HospitalStatistics stats = collectStatistics(hospital);
    
    if (type == REPORT_CONSOLE || type == REPORT_BOTH) {
        displayStatisticsReport(&stats);
    }
    
    if (type == REPORT_FILE || type == REPORT_BOTH) {
        exportStatisticsToFile(&stats, "hospital_report.txt");
    }
}

// Patient management functions
void printDepartmentDetails(Department* hospital) {
    if (hospital == NULL) return;
    
    printf("\n============================================================\n");
    printf("                    科室详细信息\n");
    printf("============================================================\n");
    
    Department* dept = hospital;
    while (dept != NULL) {
        displayDepartment(dept);
        dept = dept->next;
    }
}

void printWardDetails(Department* hospital) {
    if (hospital == NULL) return;
    
    printf("\n============================================================\n");
    printf("                    病房详细信息\n");
    printf("============================================================\n");
    
    Department* dept = hospital;
    while (dept != NULL) {
        printf("\n=== 科室: %s ===\n", dept->departmentName);
        Ward* ward = dept->wardList;
        while (ward != NULL) {
            displayWard(ward);
            ward = ward->next;
        }
        dept = dept->next;
    }
}

int admitPatient(Department* hospital, int departmentId, int wardNumber, 
                 int bedNumber, int patientId, const char* patientName) {
    if (hospital == NULL) return FAILURE;
    
    Department* dept = hospital;
    while (dept != NULL) {
        if (dept->departmentId == departmentId) {
            Ward* ward = findWardInDepartment(dept, wardNumber);
            if (ward == NULL) {
                printf("错误：科室 %d 中找不到病房 %d\n", departmentId, wardNumber);
                return FAILURE;
            }
            
            Bed* bed = findBedInWard(ward, bedNumber);
            if (bed == NULL) {
                printf("错误：病房 %d 中找不到床位 %d\n", wardNumber, bedNumber);
                return FAILURE;
            }
            
            if (!isBedAvailable(bed)) {
                printf("错误：床位 %d 已被占用\n", bedNumber);
                return FAILURE;
            }
            
            occupyBed(bed, patientId, patientName);
            ward->occupiedBeds++;
            updateDepartmentStats(dept);
            
            printf("\n✓ 病人入院登记成功！\n");
            printf("  科室: %s\n", dept->departmentName);
            printf("  病房: %d - %s\n", ward->wardNumber, ward->wardName);
            printf("  床位: %d\n", bed->bedNumber);
            printf("  病人: %s (编号: %d)\n", patientName, patientId);
            
            return SUCCESS;
        }
        dept = dept->next;
    }
    
    printf("错误：找不到科室 %d\n", departmentId);
    return FAILURE;
}

int dischargePatient(Department* hospital, int patientId) {
    if (hospital == NULL) return FAILURE;
    
    Department* dept = hospital;
    while (dept != NULL) {
        Ward* ward = dept->wardList;
        while (ward != NULL) {
            Bed* bed = ward->bedList;
            while (bed != NULL) {
                if (bed->status == STATUS_OCCUPIED && bed->patientId == patientId) {
                    printf("\n找到病人信息：\n");
                    printf("  姓名: %s\n", bed->patientName);
                    printf("  科室: %s\n", dept->departmentName);
                    printf("  病房: %d\n", ward->wardNumber);
                    printf("  床位: %d\n", bed->bedNumber);
                    
                    char confirm;
                    printf("\n确认办理出院吗？(y/n): ");
                    scanf(" %c", &confirm);
                    
                    if (confirm == 'y' || confirm == 'Y') {
                        vacateBed(bed);
                        ward->occupiedBeds--;
                        updateDepartmentStats(dept);
                        printf("\n✓ 病人出院办理成功！\n");
                        return SUCCESS;
                    } else {
                        printf("出院已取消\n");
                        return FAILURE;
                    }
                }
                bed = bed->next;
            }
            ward = ward->next;
        }
        dept = dept->next;
    }
    
    printf("错误：找不到病人编号 %d\n", patientId);
    return FAILURE;
}

int findPatientLocation(Department* hospital, int patientId, PatientInfo* info) {
    if (hospital == NULL || info == NULL) return FAILURE;
    
    Department* dept = hospital;
    while (dept != NULL) {
        Ward* ward = dept->wardList;
        while (ward != NULL) {
            Bed* bed = ward->bedList;
            while (bed != NULL) {
                if (bed->status == STATUS_OCCUPIED && bed->patientId == patientId) {
                    info->patientId = patientId;
                    strcpy(info->patientName, bed->patientName);
                    info->departmentId = dept->departmentId;
                    info->wardNumber = ward->wardNumber;
                    info->bedNumber = bed->bedNumber;
                    return SUCCESS;
                }
                bed = bed->next;
            }
            ward = ward->next;
        }
        dept = dept->next;
    }
    
    return FAILURE;
}

void displayAllPatients(Department* hospital) {
    if (hospital == NULL) return;
    
    printf("\n============================================================\n");
    printf("                    当前住院病人列表\n");
    printf("============================================================\n");
    
    int count = 0;
    Department* dept = hospital;
    
    while (dept != NULL) {
        Ward* ward = dept->wardList;
        while (ward != NULL) {
            Bed* bed = ward->bedList;
            while (bed != NULL) {
                if (bed->status == STATUS_OCCUPIED) {
                    count++;
                    printf("%2d. %-10s (编号:%-6d) %s → 病房 %d, 床位 %d\n",
                           count, bed->patientName, bed->patientId,
                           dept->departmentName, ward->wardNumber, bed->bedNumber);
                }
                bed = bed->next;
            }
            ward = ward->next;
        }
        dept = dept->next;
    }
    
    if (count == 0) {
        printf("当前没有住院病人\n");
    }
    
    printf("============================================================\n");
}

int validateTransfer(Department* hospital, int patientId, int newDepartmentId, 
                     int newWardNumber, int newBedNumber, char* errorMessage) {
    PatientInfo info;
    
    if (!findPatientLocation(hospital, patientId, &info)) {
        sprintf(errorMessage, "找不到病人编号 %d", patientId);
        return FAILURE;
    }
    
    Department* dept = hospital;
    while (dept != NULL) {
        if (dept->departmentId == newDepartmentId) {
            Ward* ward = findWardInDepartment(dept, newWardNumber);
            if (ward == NULL) {
                sprintf(errorMessage, "找不到病房 %d", newWardNumber);
                return FAILURE;
            }
            
            Bed* bed = findBedInWard(ward, newBedNumber);
            if (bed == NULL) {
                sprintf(errorMessage, "找不到床位 %d", newBedNumber);
                return FAILURE;
            }
            
            if (!isBedAvailable(bed)) {
                sprintf(errorMessage, "床位 %d 已被占用", newBedNumber);
                return FAILURE;
            }
            
            return SUCCESS;
        }
        dept = dept->next;
    }
    
    sprintf(errorMessage, "找不到科室 %d", newDepartmentId);
    return FAILURE;
}

int transferPatient(Department* hospital, int patientId, int newDepartmentId, 
                    int newWardNumber, int newBedNumber) {
    char errorMessage[200];
    
    if (!validateTransfer(hospital, patientId, newDepartmentId, newWardNumber, newBedNumber, errorMessage)) {
        printf("错误：%s\n", errorMessage);
        return FAILURE;
    }
    
    PatientInfo oldInfo;
    findPatientLocation(hospital, patientId, &oldInfo);
    
    Department* oldDept = hospital;
    while (oldDept != NULL) {
        if (oldDept->departmentId == oldInfo.departmentId) {
            Ward* oldWard = findWardInDepartment(oldDept, oldInfo.wardNumber);
            Bed* oldBed = findBedInWard(oldWard, oldInfo.bedNumber);
            vacateBed(oldBed);
            oldWard->occupiedBeds--;
            updateDepartmentStats(oldDept);
            break;
        }
        oldDept = oldDept->next;
    }
    
    Department* newDept = hospital;
    while (newDept != NULL) {
        if (newDept->departmentId == newDepartmentId) {
            Ward* newWard = findWardInDepartment(newDept, newWardNumber);
            Bed* newBed = findBedInWard(newWard, newBedNumber);
            occupyBed(newBed, patientId, oldInfo.patientName);
            newWard->occupiedBeds++;
            updateDepartmentStats(newDept);
            break;
        }
        newDept = newDept->next;
    }
    
    printf("\n✓ 病人转科转床成功！\n");
    printf("  从: %s → 病房 %d → 床位 %d\n", 
           oldInfo.patientName, oldInfo.wardNumber, oldInfo.bedNumber);
    printf("  到: 科室 %d → 病房 %d → 床位 %d\n", 
           newDepartmentId, newWardNumber, newBedNumber);
    
    return SUCCESS;
}

// Utility functions
int getValidatedIntInput(const char* prompt, int min, int max) {
    int value;
    int valid;
    
    do {
        printf("%s", prompt);
        valid = scanf("%d", &value);
        
        if (valid != 1) {
            printf("错误：请输入有效的数字！\n");
            while (getchar() != '\n'); // Clear input buffer
            valid = 0;
        } else if (value < min || value > max) {
            printf("错误：请输入 %d 到 %d 之间的数字！\n", min, max);
            valid = 0;
        } else {
            valid = 1;
        }
    } while (!valid);
    
    return value;
}

void freeHospitalSystem(Department** hospital) {
    if (hospital == NULL || *hospital == NULL) return;
    
    Department* current = *hospital;
    while (current != NULL) {
        Department* next = current->next;
        freeDepartment(current);
        current = next;
    }
    
    *hospital = NULL;
}

void printMemoryReport(void) {
    printf("\n============================================================\n");
    printf("内存清理报告\n");
    printf("============================================================\n");
    printf("✓ 所有动态分配的内存已成功释放\n");
    printf("✓ 系统资源已清理完毕\n");
    printf("============================================================\n");
}

// Menu functions
void printMainMenu(void) {
    printf("\n");
    printf("============================================================\n");
    printf("                    医院管理系统\n");
    printf("============================================================\n");
    printf("  1.  入院登记\n");
    printf("  2.  转床/转科\n");
    printf("  3.  出院办理\n");
    printf("  4.  释放床位\n");
    printf("  5.  查看科室信息\n");
    printf("  6.  查看病房信息\n");
    printf("  7.  查看所有病人\n");
    printf("  8.  生成统计报告\n");
    printf("  9.  退出系统\n");
    printf("============================================================\n");
}

Department* initializeHospital(void) {
    printf("\n============================================================\n");
    printf("                    正在初始化医院系统\n");
    printf("============================================================\n");
    
    Department* hospital = createDepartment(1, "心血管内科");
    Ward* ward101 = createWard(101, "心血管普通病房", 3);
    Ward* ward102 = createWard(102, "心血管重症监护室", 2);
    addWardToDepartment(hospital, ward101);
    addWardToDepartment(hospital, ward102);
    
    Department* dept2 = createDepartment(2, "外科");
    Ward* ward201 = createWard(201, "外科普通病房", 3);
    addWardToDepartment(dept2, ward201);
    
    Department* dept3 = createDepartment(3, "儿科");
    Ward* ward301 = createWard(301, "儿科病房", 2);
    addWardToDepartment(dept3, ward301);
    
    hospital->next = dept2;
    dept2->next = dept3;
    
    printf("\n✓ 医院系统初始化成功！\n");
    printf("  - 3 个科室\n");
    printf("  - 4 个病房\n");
    printf("  - 10 张床位\n\n");
    
    return hospital;
}

void handleAdmitPatient(Department* hospital) {
    printf("\n========== 入院登记 ==========\n");
    
    int deptId = getValidatedIntInput("请输入科室编号 (1-3): ", 1, 3);
    int wardNum = getValidatedIntInput("请输入病房号 (101/102/201/301): ", 101, 301);
    int bedNum = getValidatedIntInput("请输入床位号 (1-3): ", 1, 3);
    int patientId = getValidatedIntInput("请输入病人编号: ", 1, 99999);
    
    char patientName[MAX_NAME_LENGTH];
    printf("请输入病人姓名: ");
    scanf("%s", patientName);
    
    admitPatient(hospital, deptId, wardNum, bedNum, patientId, patientName);
}

void handleTransferPatient(Department* hospital) {
    printf("\n========== 转床/转科 ==========\n");
    
    int patientId = getValidatedIntInput("请输入要转科的病人编号: ", 1, 99999);
    int newDeptId = getValidatedIntInput("请输入新科室编号 (1-3): ", 1, 3);
    int newWardNum = getValidatedIntInput("请输入新病房号: ", 101, 301);
    int newBedNum = getValidatedIntInput("请输入新床位号: ", 1, 3);
    
    transferPatient(hospital, patientId, newDeptId, newWardNum, newBedNum);
}

void handleDischargePatient(Department* hospital) {
    printf("\n========== 出院办理 ==========\n");
    int patientId = getValidatedIntInput("请输入要出院的病人编号: ", 1, 99999);
    dischargePatient(hospital, patientId);
}

void handleReleaseBed(Department* hospital) {
    printf("\n========== 释放床位 ==========\n");
    int deptId = getValidatedIntInput("请输入科室编号: ", 1, 3);
    int wardNum = getValidatedIntInput("请输入病房号: ", 101, 301);
    int bedNum = getValidatedIntInput("请输入床位号: ", 1, 3);
    
    Department* dept = hospital;
    while (dept != NULL) {
        if (dept->departmentId == deptId) {
            Ward* ward = findWardInDepartment(dept, wardNum);
            if (ward != NULL) {
                Bed* bed = findBedInWard(ward, bedNum);
                if (bed != NULL && bed->status == STATUS_OCCUPIED) {
                    printf("\n床位 %d 当前病人: %s (编号: %d)\n", 
                           bedNum, bed->patientName, bed->patientId);
                    char confirm;
                    printf("确认释放该床位吗？(y/n): ");
                    scanf(" %c", &confirm);
                    
                    if (confirm == 'y' || confirm == 'Y') {
                        vacateBed(bed);
                        ward->occupiedBeds--;
                        updateDepartmentStats(dept);
                        printf("✓ 床位释放成功！\n");
                    }
                } else if (bed != NULL) {
                    printf("床位 %d 已经是空闲状态。\n", bedNum);
                }
            }
            break;
        }
        dept = dept->next;
    }
}

// Main function
int main(void) {
    Department* hospital = initializeHospital();
    int choice;
    
    do {
        printMainMenu();
        choice = getValidatedIntInput("请选择功能 (1-9): ", 1, 9);
        
        switch(choice) {
            case 1:
                handleAdmitPatient(hospital);
                break;
            case 2:
                handleTransferPatient(hospital);
                break;
            case 3:
                handleDischargePatient(hospital);
                break;
            case 4:
                handleReleaseBed(hospital);
                break;
            case 5:
                printDepartmentDetails(hospital);
                break;
            case 6:
                printWardDetails(hospital);
                break;
            case 7:
                displayAllPatients(hospital);
                break;
            case 8:
                generateStatisticsReport(hospital, REPORT_BOTH);
                break;
            case 9:
                printf("\n正在退出系统...\n");
                freeHospitalSystem(&hospital);
                printMemoryReport();
                printf("\n感谢使用医院管理系统！\n");
                break;
        }
    } while(choice != 9);
    
    return 0;
}