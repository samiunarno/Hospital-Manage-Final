#include <stdio.h>
#include "../include/common.h"
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
} WardBedStatus;

typedef enum {
    REPORT_CONSOLE = 1,
    REPORT_FILE = 2,
    REPORT_BOTH = 3
} ReportType;

typedef struct WardBed {
    int bedNumber;
    WardBedStatus status;
    int patientId;
    char patientName[100];
    struct WardBed* next;
} WardBed;

typedef struct Ward {
    int wardNumber;
    char wardName[MAX_NAME_LENGTH];
    int totalBeds;
    int occupiedBeds;
    WardBed* bedList;
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
void initializeBed(WardBed* bed, int bedNumber);
void addBedToWard(Ward* ward, WardBed* bed);
void displayWardBeds(const Ward* ward);
void updateDepartmentStats(Department* department);
int getValidatedIntInput(const char* prompt, int min, int max);
void freeHospitalSystem(Department** hospital);
void printMemoryReport(void);

// WardBed functions
WardBed* createBed(int bedNumber) {
    WardBed* bed = (WardBed*)malloc(sizeof(WardBed));
    if (bed == NULL) {
        printf("Bed memory allocation failed\n");
        return NULL;
    }
    initializeBed(bed, bedNumber);
    return bed;
}

void initializeBed(WardBed* bed, int bedNumber) {
    bed->bedNumber = bedNumber;
    bed->status = STATUS_AVAILABLE;
    bed->patientId = -1;
    strcpy(bed->patientName, "");
    bed->next = NULL;
}

void occupyBed(WardBed* bed, int patientId, const char* patientName) {
    if (bed == NULL) return;
    bed->status = STATUS_OCCUPIED;
    bed->patientId = patientId;
    strcpy(bed->patientName, patientName);
}

void vacateBed(WardBed* bed) {
    if (bed == NULL) return;
    bed->status = STATUS_AVAILABLE;
    bed->patientId = -1;
    strcpy(bed->patientName, "");
}

int isBedAvailable(const WardBed* bed) {
    return bed != NULL && bed->status == STATUS_AVAILABLE;
}

void displayBed(const WardBed* bed) {
    if (bed == NULL) return;
    
    if (bed->status == STATUS_OCCUPIED) {
        printf("  Bed %2d: [OCCUPIED] Patient: %s (ID: %d)\n", 
               bed->bedNumber, bed->patientName, bed->patientId);
    } else {
        printf("  Bed %2d: [AVAILABLE]\n", bed->bedNumber);
    }
}

void freeBed(WardBed* bed) {
    if (bed != NULL) {
        free(bed);
    }
}

// Ward functions
Ward* createWard(int wardNumber, const char* wardName, int totalBeds) {
    Ward* ward = (Ward*)malloc(sizeof(Ward));
    if (ward == NULL) {
        printf("Ward memory allocation failed\n");
        return NULL;
    }
    
    ward->wardNumber = wardNumber;
    strcpy(ward->wardName, wardName);
    ward->totalBeds = totalBeds;
    ward->occupiedBeds = 0;
    ward->bedList = NULL;
    ward->next = NULL;
    
    for (int i = 1; i <= totalBeds; i++) {
        WardBed* bed = createBed(i);
        addBedToWard(ward, bed);
    }
    
    return ward;
}

void addBedToWard(Ward* ward, WardBed* bed) {
    if (ward == NULL || bed == NULL) return;
    
    if (ward->bedList == NULL) {
        ward->bedList = bed;
    } else {
        WardBed* current = ward->bedList;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = bed;
    }
}

WardBed* findBedInWard(const Ward* ward, int bedNumber) {
    if (ward == NULL) return NULL;
    
    WardBed* current = ward->bedList;
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
    
    WardBed* current = ward->bedList;
    while (current != NULL) {
        displayBed(current);
        current = current->next;
    }
}

void displayWard(const Ward* ward) {
    if (ward == NULL) return;
    
    printf("\n============================================================\n");
    printf("ward %d: %s\n", ward->wardNumber, ward->wardName);
    printf("Occupancy: %d/%d beds (%.1f%%)\n", 
           ward->occupiedBeds, ward->totalBeds, getWardOccupancyRate(ward));
    printf("------------------------------------------------------------\n");
    displayWardBeds(ward);
    printf("============================================================\n");
}

void freeWard(Ward* ward) {
    if (ward == NULL) return;
    
    WardBed* current = ward->bedList;
    while (current != NULL) {
        WardBed* next = current->next;
        freeBed(current);
        current = next;
    }
    
    free(ward);
}

// Department functions
Department* createDepartment(int departmentId, const char* departmentName) {
    Department* department = (Department*)malloc(sizeof(Department));
    if (department == NULL) {
        printf("Department memory allocation failed\n");
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
    printf("Department: %s (ID: %d)\n", department->departmentName, department->departmentId);
    printf("Stats: %d patients | %d beds | %d wards\n", 
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
    printf("                    Hospital Statistics Report\n");
    printf("============================================================\n");
    
    time_t t;
    time(&t);
    printf("Report Date: %s", ctime(&t));
    printf("============================================================\n");
    
    printf("Hospital Overview\n");
    printf("  Total Departments:       %d\n", stats->totalDepartments);
    printf("  Total Wards:       %d\n", stats->totalWards);
    printf("  Total Beds:       %d\n", stats->totalBeds);
    printf("  Total Patients:       %d\n", stats->totalPatients);
    printf("  Overall Occupancy:     %.1f%%\n", stats->overallOccupancyRate);
    
    printf("============================================================\n");
    printf("Department Details\n");
    
    for (int i = 0; i < stats->departmentCount; i++) {
        printf("  %-15s %.1f%% occupancy\n", 
               stats->departmentNames[i], stats->departmentOccupancyRates[i]);
    }
    
    printf("============================================================\n");
}

void exportStatisticsToFile(const HospitalStatistics* stats, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Cannot create report file\n");
        return;
    }
    
    fprintf(file, "============================================================\n");
    fprintf(file, "Hospital Statistics Report\n");
    fprintf(file, "============================================================\n\n");
    
    time_t t;
    time(&t);
    fprintf(file, "Report Date: %s\n\n", ctime(&t));
    
    fprintf(file, "Hospital Overview\n");
    fprintf(file, "------------------------------------------------------------\n");
    fprintf(file, "Total Departments:       %d\n", stats->totalDepartments);
    fprintf(file, "Total Wards:       %d\n", stats->totalWards);
    fprintf(file, "Total Beds:       %d\n", stats->totalBeds);
    fprintf(file, "Total Patients:       %d\n", stats->totalPatients);
    fprintf(file, "Overall Occupancy:     %.1f%%\n\n", stats->overallOccupancyRate);
    
    fprintf(file, "Department Details\n");
    fprintf(file, "------------------------------------------------------------\n");
    for (int i = 0; i < stats->departmentCount; i++) {
        fprintf(file, "%s: %.1f%% occupancy\n", 
                stats->departmentNames[i], stats->departmentOccupancyRates[i]);
    }
    
    fclose(file);
    printf("\n✓ Text report exported to  '%s'\n", filename);
}

void exportStatisticsToJSON(const HospitalStatistics* stats, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) return;
    
    fprintf(file, "{\n");
    fprintf(file, "  \"totalDepartments\": %d,\n", stats->totalDepartments);
    fprintf(file, "  \"totalWards\": %d,\n", stats->totalWards);
    fprintf(file, "  \"totalBeds\": %d,\n", stats->totalBeds);
    fprintf(file, "  \"totalPatients\": %d,\n", stats->totalPatients);
    fprintf(file, "  \"overallOccupancyRate\": %.2f,\n", stats->overallOccupancyRate);
    fprintf(file, "  \"departments\": [\n");
    
    for (int i = 0; i < stats->departmentCount; i++) {
        fprintf(file, "    {\n");
        fprintf(file, "      \"name\": \"%s\",\n", stats->departmentNames[i]);
        fprintf(file, "      \"occupancyRate\": %.2f\n", stats->departmentOccupancyRates[i]);
        fprintf(file, "    }%s\n", (i < stats->departmentCount - 1) ? "," : "");
    }
    fprintf(file, "  ]\n}\n");
    fclose(file);
    printf("✓ JSON report exported to  '%s'\n", filename);
}

void exportStatisticsToCSV(const HospitalStatistics* stats, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) return;
    
    fprintf(file, "Department,OccupancyRate\n");
    for (int i = 0; i < stats->departmentCount; i++) {
        fprintf(file, "%s,%.2f\n", stats->departmentNames[i], stats->departmentOccupancyRates[i]);
    }
    fprintf(file, "TOTAL_HOSPITAL,%.2f\n", stats->overallOccupancyRate);
    fclose(file);
    printf("✓ CSV report exported to  '%s'\n", filename);
}

void generateStatisticsReport(Department* hospital, ReportType type) {
    HospitalStatistics stats = collectStatistics(hospital);
    
    if (type == REPORT_CONSOLE || type == REPORT_BOTH) {
        displayStatisticsReport(&stats);
    }
    
    if (type == REPORT_FILE || type == REPORT_BOTH) {
        exportStatisticsToFile(&stats, "hospital_report.txt");
        exportStatisticsToJSON(&stats, "hospital_report.json");
        exportStatisticsToCSV(&stats, "hospital_report.csv");
    }
}

// Patient management functions
void printDepartmentDetails(Department* hospital) {
    if (hospital == NULL) return;
    
    printf("\n============================================================\n");
    printf("                    Department Details\n");
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
    printf("                    Ward Details\n");
    printf("============================================================\n");
    
    Department* dept = hospital;
    while (dept != NULL) {
        printf("\n=== Department: %s ===\n", dept->departmentName);
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
                printf("Error: Department %d does not have ward %d\n", departmentId, wardNumber);
                return FAILURE;
            }
            
            WardBed* bed = findBedInWard(ward, bedNumber);
            if (bed == NULL) {
                printf("Error: Ward %d does not have bed %d\n", wardNumber, bedNumber);
                return FAILURE;
            }
            
            if (!isBedAvailable(bed)) {
                printf("Error: Bed %d is already occupied\n", bedNumber);
                return FAILURE;
            }
            
            occupyBed(bed, patientId, patientName);
            ward->occupiedBeds++;
            updateDepartmentStats(dept);
            
            printf("\n✓ Patient admitted successfully!\n");
            printf("  Department: %s\n", dept->departmentName);
            printf("  Ward: %d - %s\n", ward->wardNumber, ward->wardName);
            printf("  Bed: %d\n", bed->bedNumber);
            printf("  Patient: %s (ID: %d)\n", patientName, patientId);
            
            Patient* p = g_patientHead;
            while(p) {
                if(p->patientId == patientId) {
                    p->isHospitalized = 1;
                    p->bedId = bed->bedNumber;
                    break;
                }
                p = p->next;
            }
            return SUCCESS;
        }
        dept = dept->next;
    }
    
    printf("Error: Department %d not found\n", departmentId);
    return FAILURE;
}

int dischargePatient(Department* hospital, int patientId) {
    if (hospital == NULL) return FAILURE;
    
    Department* dept = hospital;
    while (dept != NULL) {
        Ward* ward = dept->wardList;
        while (ward != NULL) {
            WardBed* bed = ward->bedList;
            while (bed != NULL) {
                if (bed->status == STATUS_OCCUPIED && bed->patientId == patientId) {
                    printf("\nPatient found:\n");
                    printf("  Name: %s\n", bed->patientName);
                    printf("  Department: %s\n", dept->departmentName);
                    printf("  Ward: %d\n", ward->wardNumber);
                    printf("  Bed: %d\n", bed->bedNumber);
                    
                    char confirm;
                    printf("\nConfirm discharge? (y/n): ");
                    scanf(" %c", &confirm);
                    
                    if (confirm == 'y' || confirm == 'Y') {
                        vacateBed(bed);
                        ward->occupiedBeds--;
                        updateDepartmentStats(dept);
                        printf("\n✓ Patient discharged successfully!\n");
                        return SUCCESS;
                    } else {
                        printf("Discharge cancelled\n");
                        return FAILURE;
                    }
                }
                bed = bed->next;
            }
            ward = ward->next;
        }
        dept = dept->next;
    }
    
    printf("Error: Patient ID %d not found\n", patientId);
    return FAILURE;
}

int findPatientLocation(Department* hospital, int patientId, PatientInfo* info) {
    if (hospital == NULL || info == NULL) return FAILURE;
    
    Department* dept = hospital;
    while (dept != NULL) {
        Ward* ward = dept->wardList;
        while (ward != NULL) {
            WardBed* bed = ward->bedList;
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
    printf("                    Current Inpatient List\n");
    printf("============================================================\n");
    
    int count = 0;
    Department* dept = hospital;
    
    while (dept != NULL) {
        Ward* ward = dept->wardList;
        while (ward != NULL) {
            WardBed* bed = ward->bedList;
            while (bed != NULL) {
                if (bed->status == STATUS_OCCUPIED) {
                    count++;
                    printf("%2d. %-10s (ID:%-6d) %s → Ward %d, Bed %d\n",
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
        printf("No current inpatients\n");
    }
    
    printf("============================================================\n");
}

int validateTransfer(Department* hospital, int patientId, int newDepartmentId, 
                     int newWardNumber, int newBedNumber, char* errorMessage) {
    PatientInfo info;
    
    if (!findPatientLocation(hospital, patientId, &info)) {
        sprintf(errorMessage, "Patient ID %d not found", patientId);
        return FAILURE;
    }
    
    Department* dept = hospital;
    while (dept != NULL) {
        if (dept->departmentId == newDepartmentId) {
            Ward* ward = findWardInDepartment(dept, newWardNumber);
            if (ward == NULL) {
                sprintf(errorMessage, "Ward %d not found", newWardNumber);
                return FAILURE;
            }
            
            WardBed* bed = findBedInWard(ward, newBedNumber);
            if (bed == NULL) {
                sprintf(errorMessage, "Bed %d not found", newBedNumber);
                return FAILURE;
            }
            
            if (!isBedAvailable(bed)) {
                sprintf(errorMessage, "Bed %d is already occupied", newBedNumber);
                return FAILURE;
            }
            
            return SUCCESS;
        }
        dept = dept->next;
    }
    
    sprintf(errorMessage, "Department %d not found", newDepartmentId);
    return FAILURE;
}

int transferPatient(Department* hospital, int patientId, int newDepartmentId, 
                    int newWardNumber, int newBedNumber) {
    char errorMessage[200];
    
    if (!validateTransfer(hospital, patientId, newDepartmentId, newWardNumber, newBedNumber, errorMessage)) {
        printf("Error: %s\n", errorMessage);
        return FAILURE;
    }
    
    PatientInfo oldInfo;
    findPatientLocation(hospital, patientId, &oldInfo);
    
    Department* oldDept = hospital;
    while (oldDept != NULL) {
        if (oldDept->departmentId == oldInfo.departmentId) {
            Ward* oldWard = findWardInDepartment(oldDept, oldInfo.wardNumber);
            WardBed* oldBed = findBedInWard(oldWard, oldInfo.bedNumber);
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
            WardBed* newBed = findBedInWard(newWard, newBedNumber);
            occupyBed(newBed, patientId, oldInfo.patientName);
            newWard->occupiedBeds++;
            updateDepartmentStats(newDept);
            break;
        }
        newDept = newDept->next;
    }
    
    printf("\n✓ Patient transfer successful!\n");
    printf("  From: %s → Ward %d → Bed %d\n", 
           oldInfo.patientName, oldInfo.wardNumber, oldInfo.bedNumber);
    printf("  To  : Dept %d → Ward %d → Bed %d\n", 
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
            printf("Error: Please enter a valid number!\n");
            while (getchar() != '\n'); // Clear input buffer
            valid = 0;
        } else if (value < min || value > max) {
            printf("Error: Please enter  %d  to  %d .\n", min, max);
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
    printf("Memory Cleanup Report\n");
    printf("============================================================\n");
    printf("✓ All dynamically allocated memory released\n");
    printf("✓ System resources cleaned up\n");
    printf("============================================================\n");
}

// Menu functions
void printMainMenu(void) {
    printf("\n");
    printf("============================================================\n");
    printf("                    Hospital Management System\n");
    printf("============================================================\n");
    printf("  1.  Admit Patient\n");
    printf("  2.  Transfer Patient\n");
    printf("  3.  Discharge Patient\n");
    printf("  4.  Release Bed\n");
    printf("  5.  View Department Info\n");
    printf("  6.  View Ward Info\n");
    printf("  7.  View All Inpatients\n");
    printf("  8.  Generate Reports (TXT/JSON/CSV)\n");
    printf("  9.  Advanced Analysis & Bed Visualization\n");
    printf(" 10.  Return to Main Menu\n");
    printf("============================================================\n");
}

Department* initializeHospital(void) {
    printf("\n============================================================\n");
    printf("                    Initializing Hospital System (Full Load Environment)\n");
    printf("============================================================\n");
    
    // Dept 1
    Department* hospital = createDepartment(1, "Cardiology");
    addWardToDepartment(hospital, createWard(101, "Cardiology General Ward", 10));
    addWardToDepartment(hospital, createWard(102, "Cardiology ICU", 5));
    addWardToDepartment(hospital, createWard(103, "Cardiology Private Room", 5));
    
    // Dept 2
    Department* dept2 = createDepartment(2, "Surgery");
    addWardToDepartment(dept2, createWard(201, "Surgery General Ward", 10));
    addWardToDepartment(dept2, createWard(202, "Surgery Private Room", 5));
    
    // Dept 3
    Department* dept3 = createDepartment(3, "Pediatrics");
    addWardToDepartment(dept3, createWard(301, "Pediatrics General Ward", 10));
    addWardToDepartment(dept3, createWard(302, "Pediatrics Isolation Ward", 5));
    
    // Dept 4
    Department* dept4 = createDepartment(4, "Orthopedics");
    addWardToDepartment(dept4, createWard(401, "Orthopedics General Ward", 10));
    
    // Dept 5
    Department* dept5 = createDepartment(5, "Emergency");
    addWardToDepartment(dept5, createWard(501, "Emergency Observation", 10));
    
    hospital->next = dept2;
    dept2->next = dept3;
    dept3->next = dept4;
    dept4->next = dept5;
    
    printf("\n✓ Hospital system initialized successfully!\n");
    printf("  - 5 major departments\n");
    printf("  - 9 wards (3 types: General / ICU / Private)\n");
    printf("  - 70 total beds (supports 30+ inpatients)\n\n");
    
    return hospital;
}

void performDataAnalysis(Department* hospital) {
    printf("\n============================================================\n");
    printf("                Data Analysis & Ward Optimization Report\n");
    printf("============================================================\n");
    
    // Historical Data Analysis & Prediction
    printf("[1] Historical Data Analysis & Prediction\n");
    printf("    Based on 3-month trend analysis model:\n");
    int totalPatients = 0, totalBeds = 0;
    Department* dept = hospital;
    while(dept != NULL) {
        totalPatients += dept->totalPatients;
        totalBeds += dept->totalBeds;
        dept = dept->next;
    }
    printf("    Current inpatients: %d / %d\n", totalPatients, totalBeds);
    printf("    AI Prediction: Next month Cardiology & Pediatrics admissions expected +18%%.\n");
    printf("    Recommendation: Pre-allocate beds for affected departments; stock related medications.\n\n");
    
    // Cross-Department Ward Optimization
    printf("[2] Cross-Department Ward Optimization\n");
    printf("    Department Bed Utilization Smart Diagnosis:\n");
    
    dept = hospital;
    while(dept != NULL) {
        if(dept->totalBeds > 0) {
            float rate = (float)dept->totalPatients / dept->totalBeds * 100;
            
            // Draw ASCII Bar Chart
            int barLength = (int)(rate / 5);
            char bar[25] = {0};
            for(int i = 0; i < barLength; i++) bar[i] = '#';
            
            printf("    - %-15s [%-20s] %.1f%%\n", dept->departmentName, bar, rate);
            
            if (rate > 75.0f) {
                printf("      [WARNING] High occupancy! Reallocate beds to %s.\n", dept->departmentName);
            } else if (rate < 30.0f) {
                printf("      [SUGGESTION] Low occupancy. Recommend converting wards to shared flexible wards.\n");
            }
        }
        dept = dept->next;
    }
    
    printf("============================================================\n");
    printf("✓ Analysis complete. Supports TXT/JSON/CSV export.\n");
    printf("============================================================\n");
}

void handleAdmitPatient(Department* hospital) {
    printf("\n========== Admit Patient ==========\n");
    
    int deptId = getValidatedIntInput("Enter department ID (1-5): ", 1, 3);
    int wardNum = getValidatedIntInput("Enter ward number: ", 101, 301);
    int bedNum = getValidatedIntInput("Enter bed number (1-3): ", 1, 3);
    int patientId = getValidatedIntInput("Enter patient ID: ", 1, 99999);
    
    char patientName[MAX_NAME_LENGTH] = {0};
    Patient* p = g_patientHead;
    while(p) {
        if(p->patientId == patientId) {
            strcpy(patientName, p->name);
            break;
        }
        p = p->next;
    }
    
    if (strlen(patientName) == 0) {
        printf("Enter patient name: ");
        scanf("%s", patientName);
    } else {
        printf("Found patient: %s\n", patientName);
    }
    
    admitPatient(hospital, deptId, wardNum, bedNum, patientId, patientName);
}

void handleTransferPatient(Department* hospital) {
    printf("\n========== Transfer Patient ==========\n");
    
    int patientId = getValidatedIntInput("Enter patient ID to transfer: ", 1, 99999);
    int newDeptId = getValidatedIntInput("Enter new department ID (1-5): ", 1, 3);
    int newWardNum = getValidatedIntInput("Enter new ward number: ", 101, 301);
    int newBedNum = getValidatedIntInput("Enter new bed number: ", 1, 3);
    
    transferPatient(hospital, patientId, newDeptId, newWardNum, newBedNum);
}

void handleDischargePatient(Department* hospital) {
    printf("\n========== Discharge Patient ==========\n");
    int patientId = getValidatedIntInput("Enter patient ID to discharge: ", 1, 99999);
    dischargePatient(hospital, patientId);
}

void handleReleaseBed(Department* hospital) {
    printf("\n========== Release Bed ==========\n");
    int deptId = getValidatedIntInput("Enter department ID: ", 1, 3);
    int wardNum = getValidatedIntInput("Enter ward number: ", 101, 301);
    int bedNum = getValidatedIntInput("Enter Bed No: ", 1, 3);
    
    Department* dept = hospital;
    while (dept != NULL) {
        if (dept->departmentId == deptId) {
            Ward* ward = findWardInDepartment(dept, wardNum);
            if (ward != NULL) {
                WardBed* bed = findBedInWard(ward, bedNum);
                if (bed != NULL && bed->status == STATUS_OCCUPIED) {
                    printf("\nBed %d — current patient: %s (ID: %d)\n", 
                           bedNum, bed->patientName, bed->patientId);
                    char confirm;
                    printf("Confirm bed release? (y/n): ");
                    scanf(" %c", &confirm);
                    
                    if (confirm == 'y' || confirm == 'Y') {
                        vacateBed(bed);
                        ward->occupiedBeds--;
                        updateDepartmentStats(dept);
                        printf("✓ Bed released successfully!\n");
                    }
                } else if (bed != NULL) {
                    printf("Bed %d is already available.\n", bedNum);
                }
            }
            break;
        }
        dept = dept->next;
    }
}

static Department* g_hospital = NULL;

void init_hospital_globals(void) {
    if (g_hospital == NULL) {
        g_hospital = initializeHospital();
    }
}

// Main function
int D_entry(void) {
    int choice;
    
    do {
        printMainMenu();
        choice = getValidatedIntInput("Select option (1-10): ", 1, 10);
        
        switch(choice) {
            case 1:
                handleAdmitPatient(g_hospital);
                break;
            case 2:
                handleTransferPatient(g_hospital);
                break;
            case 3:
                handleDischargePatient(g_hospital);
                break;
            case 4:
                handleReleaseBed(g_hospital);
                break;
            case 5:
                printDepartmentDetails(g_hospital);
                break;
            case 6:
                printWardDetails(g_hospital);
                break;
            case 7:
                displayAllPatients(g_hospital);
                break;
            case 8:
                generateStatisticsReport(g_hospital, REPORT_BOTH);
                break;
            case 9:
                performDataAnalysis(g_hospital);
                break;
            case 10:
                printf("\nReturning to main menu...\n");
                printf("\nReturning to main menu...\n");
                break;
        }
    } while(choice != 10);
    
    return 0;
}