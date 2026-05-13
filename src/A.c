#include "common.h"
#include "utils.h"
#include <locale.h>

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
    ui_print_sub_title("Doctor List");
    while (head) {
        printf("%d | %s | %s | %s | Seen:%d\n",
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

static Medicine *medicine_create(int medicineId, const char *name, const char *brandName, const char *genericName, const char *alias, float price, int stock, int warningLine) {
    Medicine *m = (Medicine *)malloc(sizeof(Medicine));
    if (!m) return NULL;
    m->medicineId = medicineId;
    strncpy(m->name, name, MED_NAME_LEN - 1); m->name[MED_NAME_LEN - 1] = '\0';
    strncpy(m->brandName, brandName ? brandName : "", MED_NAME_LEN - 1); m->brandName[MED_NAME_LEN - 1] = '\0';
    strncpy(m->genericName, genericName ? genericName : "", MED_NAME_LEN - 1); m->genericName[MED_NAME_LEN - 1] = '\0';
    strncpy(m->alias, alias ? alias : "", MED_NAME_LEN - 1); m->alias[MED_NAME_LEN - 1] = '\0';
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


void patient_display_one(const Patient *patient);



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
    if (!found) printf("No matching record found!\n");
}

int patient_update(Patient *head, int patientId) {
    Patient *p = patient_find_by_id(head, patientId);
    if (!p) return 0;

    char name[NAME_LEN], gender[GENDER_LEN], dept[DEPT_LEN];
    int age;
    utils_input_string("Name: ", name, sizeof(name));
    age = utils_input_int("Age: ", 0, 150);
    utils_input_string("Gender: ", gender, sizeof(gender));
    utils_input_string("Department: ", dept, sizeof(dept));

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
    printf("Patient ID: %d\n", patient->patientId);
    printf("Name: %s\n", patient->name);
    printf("Age: %d\n", patient->age);
    printf("Gender: %s\n", patient->gender);
    printf("Department: %s\n", patient->department);
    printf("Doctor ID: %d\n", patient->doctorId);
    printf("Hospitalized: %s\n", patient->isHospitalized ? "Yes" : "No");
    printf("Bed ID: %d\n", patient->bedId);
    printf("Total Cost: %.2f\n", patient->totalCost);
}

void patient_display_all(Patient *head) {
    ui_print_sub_title("Patient List");
    if (!head) {
        printf("No patients found.\n");
        return;
    }
    printf("%-8s %-10s %-6s %-6s %-8s %-8s %-8s %-8s\n",
           "ID", "Name", "Age", "Gender", "Dept", "Doctor", "Hosp.", "Cost");
    ui_print_line();
    while (head) {
        printf("%-8d %-10s %-6d %-6s %-8s %-8d %-8s %-8.2f\n",
               head->patientId, head->name, head->age, head->gender,
               head->department, head->doctorId,
               head->isHospitalized ? "Yes" : "No", head->totalCost);
        head = head->next;
    }
}

VisitRecord *record_create(int recordId, int recordType, const char *diagnosis, float cost) {
    VisitRecord *r = (VisitRecord *)malloc(sizeof(VisitRecord));
    if (!r) return NULL;
    r->recordId = recordId;
    r->recordType = recordType;
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
    ui_print_sub_title("Visit Records");
    if (!patient->records) {
        printf("No visit records.\n");
        return;
    }
    VisitRecord *cur = patient->records;
    while (cur) {
        const char* typeStr = "Unknown";
        if (cur->recordType == 1) typeStr = "Registration";
        else if (cur->recordType == 2) typeStr = "Consultation";
        else if (cur->recordType == 3) typeStr = "Examination";
        else if (cur->recordType == 4) typeStr = "Hosp.";
        printf("Record ID: %d | Type: %s | Diagnosis: %s | Cost: %.2f\n", cur->recordId, typeStr, cur->diagnosis, cur->cost);
        cur = cur->next;
    }
}

void patient_register(Patient **head, int *nextPatientId) {
    char name[NAME_LEN], gender[GENDER_LEN], dept[DEPT_LEN];
    int age;
    utils_input_string("Name: ", name, sizeof(name));
    age = utils_input_int("Age: ", 0, 150);
    utils_input_string("Gender: ", gender, sizeof(gender));
    utils_input_string("Department: ", dept, sizeof(dept));

    Patient *p = patient_create((*nextPatientId)++, name, age, gender, dept);
    if (!p || !patient_add(head, p)) {
        printf("Operation failed!\n");
        free(p);
        return;
    }
    printf("Patient added! Patient ID: %d\n", p->patientId);
}

static const char *registration_type_text(int registerType) {
    return registerType == REGISTER_BOOK ? "Appointment" : "Walk-in";
}

static const char *registration_status_text(int status) {
    switch (status) {
        case STATUS_WAIT: return "Waiting";
        case STATUS_CALL: return "Called";
        case STATUS_TREATING: return "In Consultation";
        case STATUS_FINISH: return "Completed";
        default: return "UnknownStatus";
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
    printf("RegistrationID：%d\n", registration->registerId);
    printf("Patient ID: %d\n", registration->patientId);
    printf("Doctor ID: %d\n", registration->doctorId);
    printf("Department: %s\n", registration->department);
    printf("Type: %s\n", registration_type_text(registration->registerType));
    printf("RegistrationQueue#：%d\n", registration->registerTime);
    printf("Status: %s\n", registration_status_text(registration->status));
}

void registration_display_all(Registration *head) {
    ui_print_sub_title("Registration Records");
    if (!head) {
        printf("No registration records.\n");
        return;
    }
    printf("%-8s %-8s %-8s %-8s %-10s %-10s %-10s\n",
           "Reg.ID", "Patient", "Doctor", "Dept", "Type", "Queue#", "Status");
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
    if (!found) printf("No registration records for this patient.\n");
}

static int patient_register_common(Patient *pHead, Doctor *dHead, Registration **regHead,
                                   int *nextRegisterId, int *nextRegisterTime, int registerType) {
    if (!pHead) {
        printf("No patients found. Please add a patient first.\n");
        return 0;
    }
    if (!dHead) {
        printf("No doctors available. Cannot register.\n");
        return 0;
    }

    patient_display_all(pHead);
    int patientId = utils_input_int("Enter Patient ID: ", 1, 999999);
    Patient *patient = patient_find_by_id(pHead, patientId);
    if (!patient) {
        printf("Patient not found!\n");
        return 0;
    }

    printf("Available doctors:\n");
    doctor_display_all(dHead);
    int doctorId = utils_input_int("Enter Doctor ID: ", 1, 999999);
    Doctor *doctor = doctor_find_by_id(dHead, doctorId);
    if (!doctor) {
        printf("Doctor not found!\n");
        return 0;
    }

    if (strcmp(patient->department, doctor->department) != 0) {
        printf("Registration failed: Patient and doctor departments do not match.\n");
        return 0;
    }

    Registration *reg = registration_create((*nextRegisterId)++, patient->patientId,
                                            doctor->doctorId, patient->department,
                                            registerType, (*nextRegisterTime)++);
    if (!reg || !registration_add(regHead, reg)) {
        printf("Registration failed!\n");
        free(reg);
        return 0;
    }

    patient->doctorId = doctor->doctorId;
    printf("%s successful! Registration ID: %d, Status: %s\n",
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
        ui_print_sub_title("Registration Record Search");
        printf("1. Search by Registration ID\n");
        printf("2. Search by Patient ID\n");
        printf("3. Show all registrations\n");
        printf("0. Back\n");
        ui_print_line();
        choice = utils_input_int("Select: ", 0, 3);
        if (choice == 0) break;

        if (choice == 1) {
            int registerId = utils_input_int("Enter RegistrationID：", 1, 999999);
            Registration *reg = registration_find_by_id(regHead, registerId);
            if (!reg) printf("Registration record not found!\n");
            else registration_display_one(reg);
        } else if (choice == 2) {
            int patientId = utils_input_int("Enter Patient ID: ", 1, 999999);
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
        ui_print_sub_title("Patient & Registration Management");
        printf("1. Add New Patient\n");
        printf("2. Book Appointment\n");
        printf("3. Walk-in Registration\n");
        printf("4. Search Patient\n");
        printf("5. Update Patient Info\n");
        printf("6. Delete Patient\n");
        printf("7. Add Visit Record\n");
        printf("8. Display All Patients\n");
        printf("9. View Registration Records\n");
        printf("10. Sort by Age\n");
        printf("0. Back to Main Menu\n");
        ui_print_line();
        choice = utils_input_int("Select: ", 0, 10);
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
                id = utils_input_int("Enter Patient ID (0 to search by name): ", 0, 999999);
                if (id == 0) {
                    utils_input_string("Name: ", name, sizeof(name));
                    patient_find_by_name(*pHead, name);
                } else {
                    p = patient_find_by_id(*pHead, id);
                    if (!p) printf("No matching record found!\n");
                    else {
                        patient_display_one(p);
                        patient_show_records(p);
                    }
                }
                break;
            case 5:
                id = utils_input_int("Enter Patient ID: ", 1, 999999);
                printf(patient_update(*pHead, id) ? "Updated successfully!\n" : "No matching record found!\n");
                break;
            case 6:
                id = utils_input_int("Enter Patient ID: ", 1, 999999);
                printf(patient_delete(pHead, id) ? "Deleted successfully!\n" : "No matching record found!\n");
                break;
            case 7:
                id = utils_input_int("Enter Patient ID: ", 1, 999999);
                p = patient_find_by_id(*pHead, id);
                if (!p) {
                    printf("No matching record found!\n");
                    break;
                }
                int rType = utils_input_int("Enter Record Type (1:Registration, 2:Consultation, 3:Examination, 4:Hosp.)：", 1, 4);
                utils_input_string("Enter Diagnosis: ", diagnosis, sizeof(diagnosis));
                cost = utils_input_float("Enter Cost: ", 0.0f, 1000000.0f);
                if (patient_add_record(p, record_create((*nextRecordId)++, rType, diagnosis, cost))) {
                    p->totalCost += cost;
                    printf("Added successfully!\n");
                } else {
                    printf("Operation failed!\n");
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
                printf("Sort successful!\n");
                patient_display_all(*pHead);
                break;
        }
        utils_wait_enter();
    }
}

/************************ A：init.c ************************/

void init_patients(Patient **pHead, int *nextPatientId, int *nextRecordId) {
    Patient *p;
    char nameBuf[NAME_LEN];
    int i;
    for (i = 1; i <= 100; i++) {
        sprintf(nameBuf, "Outpatient%d", i);
        p = patient_create((*nextPatientId)++, nameBuf, 20 + (i % 40), (i % 2 == 0) ? "M" : "F", "Internal Medicine");
        patient_add(pHead, p);
    }
    for (i = 1; i <= 30; i++) {
        sprintf(nameBuf, "Inpatient%d", i);
        p = patient_create((*nextPatientId)++, nameBuf, 30 + (i % 40), (i % 2 == 0) ? "F" : "M", "Surgery");
        p->isHospitalized = 1;
        p->bedId = 5000 + i;
        patient_add(pHead, p);
    }
}

void init_doctors(Doctor **dHead, int *nextDoctorId) {
    const char* depts[] = {"Internal Medicine", "Surgery", "Pediatrics", "Orthopedics", "Emergency"};
    char nameBuf[NAME_LEN];
    for (int i = 1; i <= 20; i++) {
        sprintf(nameBuf, "Doctor%d", i);
        doctor_add(dHead, doctor_create((*nextDoctorId)++, nameBuf, depts[i % 5], "Attending Physician"));
    }
}

void init_medicines(Medicine **mHead, int *nextMedicineId) {
    char nameBuf[MED_NAME_LEN];
    char brandBuf[MED_NAME_LEN];
    char genBuf[MED_NAME_LEN];
    char aliasBuf[MED_NAME_LEN];
    for (int i = 1; i <= 25; i++) {
        sprintf(nameBuf, "Medicine%d", i);
        sprintf(brandBuf, "Brand%d", i);
        sprintf(genBuf, "Generic%d", i);
        sprintf(aliasBuf, "Alias%d", i);
        medicine_add(mHead, medicine_create((*nextMedicineId)++, nameBuf, brandBuf, genBuf, aliasBuf, 10.0f + i, 100, 10));
    }
}

void init_beds(Bed **bHead, int *nextBedId) {
    int i;
    for (i = 0; i < 20; ++i) bed_add(bHead, bed_create((*nextBedId)++, "General Ward"));
    for (i = 0; i < 10; ++i) bed_add(bHead, bed_create((*nextBedId)++, "Private Room"));
    for (i = 0; i < 10; ++i) bed_add(bHead, bed_create((*nextBedId)++, "ICU"));
}

void init_system_globals(void) {
    if (g_patientHead != NULL) return; // Only init once
    
    init_patients(&g_patientHead, &g_nextPatientId, &g_nextRecordId);
    init_doctors(&g_doctorHead, &g_nextDoctorId);
    init_medicines(&g_medicineHead, &g_nextMedicineId);
}

/************************ A：main.c ************************/

int A_entry(void) {
    setlocale(LC_ALL, "");
    
    patient_menu(&g_patientHead, g_doctorHead, &g_regHead,
                 &g_nextPatientId, &g_nextRegisterId,
                 &g_nextRegisterTime, &g_nextRecordId);

    // Data is preserved globally, do not free here!
    return 0;
}
