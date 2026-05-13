#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>  

#define MAX_NAME_LEN 20
#define MAX_DEP_LEN 20
#define MAX_DIAG_LEN 100

#include "../include/common.h"

// Note: Removed local struct definitions (VisitRecord, PrescriptionItem, Prescription, Patient, Doctor, Medicine, Bed, Registration, QueueNode)
// since they are now unified in common.h

void addIntoQueue(int registerId, int patientId, int doctorId);

// Removed static heads since we use globals from common.h


// ===================== Department & Doctor Query =====================
void queryAllDepartment() {
    if (g_doctorHead == NULL) {
        printf("No doctor data available!\n");
        return;
    }
    char deptList[100][MAX_DEP_LEN];
    int deptCount = 0;
    Doctor* p = g_doctorHead;
    printf("=== All Hospital Departments ===\n");
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
    if (g_doctorHead == NULL) {
        printf("No doctor data available!\n");
        return;
    }
    Doctor* p = g_doctorHead;
    int count = 0;
    printf("=== %s Department Doctors ===\n", deptName);
    while (p != NULL) {
        if (strcmp(p->department, deptName) == 0) {
            count++;
            printf("DoctorID: %d | Name: %s | Title: %s | Patients seen: %d\n",
                   p->doctorId, p->name, p->title, p->patientCount);
        }
        p = p->next;
    }
    if (count == 0) printf("No doctors in this department!\n");
}

Doctor* queryDoctorById(int doctorId) {
    Doctor* p = g_doctorHead;
    while (p != NULL) {
        if (p->doctorId == doctorId) return p;
        p = p->next;
    }
    return NULL;
}

// ===================== Patient Visit Records =====================
VisitRecord* createVisitRecord(int recordId, int recordType, const char* diagnosis, double cost) {
    VisitRecord* newNode = (VisitRecord*)malloc(sizeof(VisitRecord));
    newNode->recordId = recordId;
    newNode->recordType = recordType;
    strcpy(newNode->diagnosis, diagnosis);
    newNode->cost = cost;
    newNode->next = NULL;
    return newNode;
}

void addPatientVisit(int patientId, int recordId, int recordType, const char* diagnosis, double cost) {
    Patient* p = g_patientHead;
    while (p != NULL) {
        if (p->patientId == patientId) {
            VisitRecord* newRecord = createVisitRecord(recordId, recordType, diagnosis, cost);
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
            printf("Visit RecordsAdded successfully!PatientTotal Cost: %.2f\n", p->totalCost);
            return;
        }
        p = p->next;
    }
    printf("Patient not found, record add failed!\n");
}

void queryPatientRecord(int patientId) {
    Patient* p = g_patientHead;
    while (p != NULL) {
        if (p->patientId == patientId) {
            printf("=== Patient [%s] (ID:%d) All Records ===\n", p->name, p->patientId);
            if (p->records == NULL) {
                printf("No visit records!\n");
                return;
            }
            VisitRecord* r = p->records;
            while (r != NULL) {
                const char* typeStr = "Unknown";
                if (r->recordType == 1) typeStr = "Registration";
                else if (r->recordType == 2) typeStr = "Consultation";
                else if (r->recordType == 3) typeStr = "Examination";
                else if (r->recordType == 4) typeStr = "Hosp.";
                printf("Record ID: %d | Type: %s | Diagnosis: %s | Cost: %.2f\n",
                       r->recordId, typeStr, r->diagnosis, r->cost);
                r = r->next;
            }
            return;
        }
        p = p->next;
    }
    printf("Patient not found!\n");
}

void modifyPatientRecord(int patientId, int recordId, const char* newDiag, double newCost) {
    Patient* p = g_patientHead;
    while (p != NULL) {
        if (p->patientId == patientId) {
            VisitRecord* r = p->records;
            while (r != NULL) {
                if (r->recordId == recordId) {
                    p->totalCost = p->totalCost - r->cost + newCost;
                    strcpy(r->diagnosis, newDiag);
                    r->cost = newCost;
                    printf("Record updated! Patient total cost: %.2f\n", p->totalCost);
                    return;
                }
                r = r->next;
            }
            printf("Record not found!\n");
            return;
        }
        p = p->next;
    }
    printf("Patient not found!\n");
}

// ===================== Hospitalization Status Management =====================
void setPatientHospitalStatus(int patientId, bool isHosp, int bedId) {
    Patient* p = g_patientHead;
    while (p != NULL) {
        if (p->patientId == patientId) {
            p->isHospitalized = isHosp;
            p->bedId = bedId;
            Bed* b = g_bedHead;
            while (b != NULL) {
                if (b->bedId == bedId) {
                    b->isOccupied = isHosp;
                    b->patientId = isHosp ? patientId : -1;
                    break;
                }
                b = b->next;
            }
            printf("Status updated: %s | Bed ID: %d\n",
                   isHosp ? "Admitted" : "Discharged", bedId);
            return;
        }
        p = p->next;
    }
    printf("Patient not found!\n");
}

void queryPatientHospitalStatus(int patientId) {
    Patient* p = g_patientHead;
    while (p != NULL) {
        if (p->patientId == patientId) {
            printf("Patient [%s] (ID:%d): %s | Bed ID: %d\n",
                   p->name, p->patientId,
                   p->isHospitalized ? "Admitted" : "Not Admitted",
                   p->bedId);
            return;
        }
        p = p->next;
    }
    printf("Patient not found!\n");
}

// ===================== Registration: Create Registration =====================
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
    Patient* p = g_patientHead;
    bool patientExist = false;
    while (p != NULL) {
        if (p->patientId == patientId) {
            patientExist = true;
            break;
        }
        p = p->next;
    }

    if (!patientExist) {
        printf("Patient not found, registration failed!\n");
        return;
    }
    if (queryDoctorById(doctorId) == NULL) {
        printf("Doctor not found, registration failed!\n");
        return;
    }

    Registration* newReg = createRegistration(registerId, patientId, doctorId, dept, regType, regTime);
    if (g_regHead == NULL)
        g_regHead = newReg;
    else {
        Registration* t = g_regHead;
        while (t->next != NULL) t = t->next;
        t->next = newReg;
    }
    printf("Registered! Registration ID: %d, Status: Waiting\n", registerId);

  addIntoQueue(registerId, patientId, doctorId);

}

// ===================== Module B: Waiting Queue =====================
void addIntoQueue(int registerId, int patientId, int doctorId) {
    static int qid = 1000;
    QueueNode* q = (QueueNode*)malloc(sizeof(QueueNode));
    q->queueId = qid++;
    q->registerId = registerId;
    q->patientId = patientId;
    q->doctorId = doctorId;
    q->status = 0;
    q->next = NULL;

    if (g_queueHead == NULL)
        g_queueHead = q;
    else {
        QueueNode* t = g_queueHead;
        while (t->next != NULL) t = t->next;
        t->next = q;
    }
}

// View all waiting queue
void queryAllQueue() {
    if (g_queueHead == NULL) {
        printf("Queue is empty!\n");
        return;
    }
    printf("=== Current Doctor Queue ===\n");
    QueueNode* t = g_queueHead;
    while (t != NULL) {
        char stat[10];
        switch (t->status) {
            case 0: strcpy(stat, "Waiting"); break;
            case 1: strcpy(stat, "Called"); break;
            case 2: strcpy(stat, "In Consultation"); break;
            case 3: strcpy(stat, "Completed"); break;
        }
        printf("Queue#: %d | Reg.ID: %d | PatientID: %d | DoctorID: %d | Status: %s\n",
               t->queueId, t->registerId, t->patientId, t->doctorId, stat);
        t = t->next;
    }
}

// Call next patient
void callNextPatient() {
    if (g_queueHead == NULL) {
        printf("No waiting patients!\n");
        return;
    }
    QueueNode* first = g_queueHead;
    first->status = 2;

    Registration* r = g_regHead;
    while (r != NULL) {
        if (r->registerId == first->registerId) {
            r->status = 2;
            break;
        }
        r = r->next;
    }
    printf("Patient called! Now in consultation.\n");
}

// Complete consultation
void finishCurrentVisit() {
    if (g_queueHead == NULL) {
        printf("No patient currently in consultation!\n");
        return;
    }
    QueueNode* current = g_queueHead;
    current->status = 3;

    Registration* r = g_regHead;
    while (r != NULL) {
        if (r->registerId == current->registerId) {
            r->status = 3;
            break;
        }
        r = r->next;
    }
    printf("Consultation completed!\n");
}

// ===================== Main entry =====================
int B_entry(void) {
    int op;
    while (1) {
        printf("\n===== Module B — Queue & Consultation =====\n");
        printf("1. View All Departments\n");
        printf("2. View Internal Medicine Doctors\n");
        printf("3. Walk-in Registration (Test: Patient 1001, Doctor 2001)\n");
        printf("4. View Queue\n");
        printf("5. Call Next Patient\n");
        printf("6. End Current Visit\n");
        printf("7. View Patient Records (ID 1001)\n");
        printf("0. Back to Main Menu\n");
        printf("Select: ");
        if (scanf("%d", &op) != 1) {
            while (getchar() != '\n');
            continue;
        }
        
        if (op == 1) queryAllDepartment();
        else if (op == 2) queryDoctorByDept("Internal Medicine");
        else if (op == 3) {
            patientRegister(g_nextRegisterId++, 1001, 2001, "Internal Medicine", 0, g_nextRegisterTime++);
        }
        else if (op == 4) queryAllQueue();
        else if (op == 5) callNextPatient();
        else if (op == 6) finishCurrentVisit();
        else if (op == 7) queryPatientRecord(1001);
        else if (op == 0) break;
        else printf("Invalid option\n");
    }
    return 0;
}
