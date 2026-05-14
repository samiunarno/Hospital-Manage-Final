#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../include/auth.h"
#include "../include/common.h"
#include "../include/lang.h"

extern int A_entry(void);
extern int B_entry(void);
extern int C_entry(void);
extern int D_entry(void);
extern void init_system_globals(void);
extern void init_hospital_globals(void);

/* ===== Language (0=English 1=Chinese) ===== */
int g_lang = 0;

/* ===== Global shared data ===== */
Patient      *g_patientHead       = NULL;
Doctor       *g_doctorHead        = NULL;
Medicine     *g_medicineHead      = NULL;
Bed          *g_bedHead           = NULL;
Registration *g_regHead           = NULL;
QueueNode    *g_queueHead         = NULL;
Prescription *g_presHead          = NULL;

int g_nextPatientId      = 1001;
int g_nextDoctorId       = 2001;
int g_nextMedicineId     = 3001;
int g_nextPrescriptionId = 4001;
int g_nextBedId          = 5001;
int g_nextRecordId       = 6001;
int g_nextRegisterId     = 7001;
int g_nextQueueId        = 8001;
int g_nextRegisterTime   = 1;

/* ===== Memory cleanup helpers ===== */
extern void patient_free_all(Patient *head);
static void free_all_lists(void) {
    /* Free patients (including visit records) */
    patient_free_all(g_patientHead); g_patientHead = NULL;
    /* Free doctors */
    Doctor *d = g_doctorHead;
    while (d) { Doctor *t=d; d=d->next; free(t); } g_doctorHead = NULL;
    /* Free medicines */
    Medicine *m = g_medicineHead;
    while (m) { Medicine *t=m; m=m->next; free(t); } g_medicineHead = NULL;
    /* Free beds */
    Bed *b = g_bedHead;
    while (b) { Bed *t=b; b=b->next; free(t); } g_bedHead = NULL;
    /* Free registrations */
    Registration *r = g_regHead;
    while (r) { Registration *t=r; r=r->next; free(t); } g_regHead = NULL;
    /* Free queue */
    QueueNode *q = g_queueHead;
    while (q) { QueueNode *t=q; q=q->next; free(t); } g_queueHead = NULL;
    /* Free prescriptions + items */
    Prescription *pr = g_presHead;
    while (pr) {
        PrescriptionItem *it = pr->items;
        while (it) { PrescriptionItem *ti=it; it=it->next; free(ti); }
        Prescription *tp=pr; pr=pr->next; free(tp);
    } g_presHead = NULL;
}

/* ===== Medicine sort helpers ===== */
static void medicine_sort_by_price(Medicine **head) {
    if (!head || !*head) return;
    int swapped; Medicine *ptr; Medicine *lptr=NULL;
    do {
        swapped=0; ptr=*head;
        while (ptr->next != lptr) {
            if (ptr->price > ptr->next->price) {
                /* swap fields */
                int tmpId=ptr->medicineId; ptr->medicineId=ptr->next->medicineId; ptr->next->medicineId=tmpId;
                char tmp[MED_NAME_LEN];
                #define SWAPF(f) strcpy(tmp,ptr->f);strcpy(ptr->f,ptr->next->f);strcpy(ptr->next->f,tmp)
                SWAPF(name); SWAPF(brandName); SWAPF(genericName); SWAPF(alias);
                #undef SWAPF
                float tf=ptr->price; ptr->price=ptr->next->price; ptr->next->price=tf;
                int ti=ptr->stock; ptr->stock=ptr->next->stock; ptr->next->stock=ti;
                ti=ptr->warningLine; ptr->warningLine=ptr->next->warningLine; ptr->next->warningLine=ti;
                swapped=1;
            }
            ptr=ptr->next;
        } lptr=ptr;
    } while(swapped);
}
static void medicine_sort_by_stock(Medicine **head) {
    if (!head || !*head) return;
    int swapped; Medicine *ptr; Medicine *lptr=NULL;
    do {
        swapped=0; ptr=*head;
        while (ptr->next != lptr) {
            if (ptr->stock > ptr->next->stock) {
                int tmpId=ptr->medicineId; ptr->medicineId=ptr->next->medicineId; ptr->next->medicineId=tmpId;
                char tmp[MED_NAME_LEN];
                #define SWAPF(f) strcpy(tmp,ptr->f);strcpy(ptr->f,ptr->next->f);strcpy(ptr->next->f,tmp)
                SWAPF(name); SWAPF(brandName); SWAPF(genericName); SWAPF(alias);
                #undef SWAPF
                float tf=ptr->price; ptr->price=ptr->next->price; ptr->next->price=tf;
                int ti=ptr->stock; ptr->stock=ptr->next->stock; ptr->next->stock=ti;
                ti=ptr->warningLine; ptr->warningLine=ptr->next->warningLine; ptr->next->warningLine=ti;
                swapped=1;
            }
            ptr=ptr->next;
        } lptr=ptr;
    } while(swapped);
}

/* ===== Helpers ===== */
static void flush(void) {
    int c; while ((c = getchar()) != '\n' && c != EOF);
}

static int get_int(const char *prompt, int lo, int hi) {
    int v; int ok;
    do {
        printf("%s", prompt);
        ok = scanf("%d", &v);
        flush();
        if (!ok) { printf(L("[!] Enter a number.\n","[!] 请输入数字。\n")); ok = 0; continue; }
        if (v < lo || v > hi) { printf(L("[!] Enter %d-%d.\n","[!] 请输入 %d 到 %d。\n"), lo, hi); ok = 0; }
    } while (!ok);
    return v;
}

static void get_str(const char *prompt, char *buf, int maxlen) {
    printf("%s", prompt);
    if (fgets(buf, maxlen, stdin))
        buf[strcspn(buf, "\r\n")] = 0;
}

/* ===== Auth helpers ===== */
static void do_register_public(void) {
    char username[MAX_USERNAME_LEN], password[MAX_PASSWORD_LEN];
    char full_name[MAX_NAME_LEN], national_id[MAX_NID_LEN];
    int rc;

    printf("\n==============================\n");
    printf("       USER REGISTRATION\n");
    printf("==============================\n");
    printf("Username   : "); if (scanf("%49s", username)!=1){flush();return;} flush();
    printf("Password   : "); if (scanf("%63s", password)!=1){flush();return;} flush();
    get_str("Full Name  : ", full_name, MAX_NAME_LEN);
    printf("National ID: "); if (scanf("%49s", national_id)!=1){flush();return;} flush();
    rc = get_int("Role (1=Receptionist 2=Doctor 3=Patient): ", 1, 3);

    UserRole role = (rc==1)?ROLE_RECEPTIONIST:(rc==2)?ROLE_DOCTOR:ROLE_PATIENT;

    if (auth_register(username, password, full_name, national_id, role)) {
        printf("[+] Registered! Waiting for admin approval before you can login.\n");
    } else {
        printf("[-] Failed: username or National ID already exists, or fields empty.\n");
    }
}

/* ===== ADMIN: User Management ===== */
static void admin_user_management(void) {
    while (1) {
        printf("\n============================================\n");
        printf("         ADMIN — USER MANAGEMENT\n");
        printf("============================================\n");
        printf("  Pending approvals: %d\n", auth_get_pending_count());
        printf("  Total users      : %d\n", auth_get_user_count());
        printf("--------------------------------------------\n");
        printf("  1. View Pending Users\n");
        printf("  2. Approve User\n");
        printf("  3. Reject User\n");
        printf("  4. View All Approved Users\n");
        printf("  5. Create New User (auto-approved)\n");
        printf("  6. Update User Role\n");
        printf("  7. Delete User\n");
        printf("  0. Back\n");
        printf("============================================\n");

        int ch = get_int("Select: ", 0, 7);
        if (ch == 0) break;

        if (ch == 1) {
            User list[500]; int cnt=0;
            auth_get_pending_users(list, &cnt);
            printf("\n--- Pending Users (%d) ---\n", cnt);
            for (int i=0;i<cnt;i++)
                printf("  %d. %-20s | %-30s | %s\n", i+1, list[i].username, list[i].full_name, auth_role_to_string(list[i].role));
            if (!cnt) printf("  (none)\n");

        } else if (ch == 2) {
            char u[MAX_USERNAME_LEN];
            printf("Username to approve: "); scanf("%49s",u); flush();
            printf(auth_approve_user(u) ? "[+] Approved: %s\n" : "[-] Not found or already approved.\n", u);

        } else if (ch == 3) {
            char u[MAX_USERNAME_LEN];
            printf("Username to reject: "); scanf("%49s",u); flush();
            printf(auth_reject_user(u) ? "[+] Rejected & removed: %s\n" : "[-] Not found.\n", u);

        } else if (ch == 4) {
            User list[500]; int cnt=0;
            auth_get_approved_users(list, &cnt);
            printf("\n--- Approved Users (%d) ---\n", cnt);
            for (int i=0;i<cnt;i++)
                printf("  %d. %-20s | %-30s | %s\n", i+1, list[i].username, list[i].full_name, auth_role_to_string(list[i].role));

        } else if (ch == 5) {
            char username[MAX_USERNAME_LEN], password[MAX_PASSWORD_LEN];
            char full_name[MAX_NAME_LEN], national_id[MAX_NID_LEN];
            printf("Username   : "); scanf("%49s",username); flush();
            printf("Password   : "); scanf("%63s",password); flush();
            get_str("Full Name  : ", full_name, MAX_NAME_LEN);
            printf("National ID: "); scanf("%49s",national_id); flush();
            int rc2 = get_int("Role (0=Admin 1=Receptionist 2=Doctor 3=Patient): ", 0, 3);
            UserRole role = (rc2==0)?ROLE_ADMIN:(rc2==1)?ROLE_RECEPTIONIST:(rc2==2)?ROLE_DOCTOR:ROLE_PATIENT;
            if (auth_register(username, password, full_name, national_id, role)) {
                auth_approve_user(username);
                printf("[+] User created & approved: %s\n", username);
            } else {
                printf("[-] Failed (username/NID exists).\n");
            }

        } else if (ch == 6) {
            char u[MAX_USERNAME_LEN];
            printf("Username: "); scanf("%49s",u); flush();
            int rc3 = get_int("New Role (0=Admin 1=Receptionist 2=Doctor 3=Patient): ", 0, 3);
            UserRole role = (rc3==0)?ROLE_ADMIN:(rc3==1)?ROLE_RECEPTIONIST:(rc3==2)?ROLE_DOCTOR:ROLE_PATIENT;
            printf(auth_update_user_role(u,role) ? "[+] Role updated.\n" : "[-] User not found.\n");

        } else if (ch == 7) {
            char u[MAX_USERNAME_LEN];
            printf("Username to delete: "); scanf("%49s",u); flush();
            if (auth_delete_user(u))
                printf("[+] Deleted: %s\n", u);
            else
                printf("[-] Not found or cannot delete last admin.\n");
        }
    }
}

/* ===== ADMIN: Patient CRUD ===== */
static void admin_patient_management(void) {
    while (1) {
        printf("\n============================================\n");
        printf("         ADMIN — PATIENT MANAGEMENT\n");
        printf("============================================\n");
        printf("  1. %s\n", L("List All Patients","查看所有患者"));
        printf("  2. %s\n", L("Add New Patient","新增患者"));
        printf("  3. %s\n", L("Search Patient by ID","按编号查询"));
        printf("  4. %s\n", L("Delete Patient by ID","按编号删除"));
        printf("  5. %s\n", L("Search Patients by Department","按科室查询患者"));
        printf("  6. %s\n", L("View Patient Cost Summary","查看患者费用明细"));
        printf("  0. %s\n", L("Back","返回"));
        int ch = get_int(L("Select: ","请选择："), 0, 6);
        if (ch == 0) break;

        if (ch == 1) {
            Patient *p = g_patientHead; int n=0;
            printf("\n%-6s %-20s %-4s %-6s %-10s %-8s\n","ID","Name","Age","Gender","Dept","Hospit.");
            printf("---------------------------------------------------------------\n");
            while (p) {
                printf("%-6d %-20s %-4d %-6s %-10s %-8s\n",
                       p->patientId, p->name, p->age, p->gender,
                       p->department, p->isHospitalized?"YES":"NO");
                p=p->next; n++;
            }
            printf("Total: %d\n", n);

        } else if (ch == 2) {
            Patient *np = (Patient*)calloc(1,sizeof(Patient));
            np->patientId = g_nextPatientId++;
            get_str("Name      : ", np->name, NAME_LEN);
            np->age = get_int("Age       : ", 0, 150);
            get_str("Gender    : ", np->gender, GENDER_LEN);
            get_str("Department: ", np->department, DEPT_LEN);
            np->isHospitalized = 0; np->bedId = -1;
            np->records = NULL; np->next = NULL;
            /* append */
            if (!g_patientHead) { g_patientHead = np; }
            else { Patient *t=g_patientHead; while(t->next) t=t->next; t->next=np; }
            printf("[+] Patient added: ID=%d\n", np->patientId);

        } else if (ch == 3) {
            int id = get_int("Patient ID: ", 1, 999999);
            Patient *p = g_patientHead;
            while (p && p->patientId != id) p=p->next;
            if (p) {
                printf("\n--- Patient Info ---\n");
                printf("ID    : %d\nName  : %s\nAge   : %d\nGender: %s\nDept  : %s\nHosp. : %s\nBed   : %d\n",
                       p->patientId, p->name, p->age, p->gender, p->department,
                       p->isHospitalized?"YES":"NO", p->bedId);
            } else printf("[-] Not found.\n");

        } else if (ch == 4) {
            int id = get_int("Patient ID to delete: ", 1, 999999);
            Patient *p=g_patientHead, *prev=NULL;
            while (p && p->patientId!=id){prev=p; p=p->next;}
            if (p){
                if(prev) prev->next=p->next; else g_patientHead=p->next;
                free(p); printf("[+] Deleted.\n");
            } else printf("[-] Not found.\n");

        /* ---- REQ 1d: Query by Department ---- */
        } else if (ch == 5) {
            char dept[DEPT_LEN];
            get_str(L("Department name: ","科室名称："), dept, DEPT_LEN);
            int n=0;
            printf("\n%-6s %-20s %-4s %-6s %-8s %-8s\n","ID","Name","Age","Gender","Hosp.","Cost");
            printf("-------------------------------------------------------\n");
            Patient *p2=g_patientHead;
            while (p2) {
                if (strstr(p2->department, dept) || strcmp(p2->department, dept)==0) {
                    printf("%-6d %-20s %-4d %-6s %-8s %-8.2f\n",
                           p2->patientId, p2->name, p2->age, p2->gender,
                           p2->isHospitalized?"YES":"NO", p2->totalCost);
                    n++;
                }
                p2=p2->next;
            }
            if (g_lang) printf("科室 '%s' 共 %d 名患者\n", dept, n);
            else printf("Found: %d patient(s) in '%s'\n", n, dept);

        /* ---- REQ 5c: Cost summary per patient ---- */
        } else if (ch == 6) {
            int id = get_int(L("Patient ID: ","患者编号："), 1, 999999);
            Patient *p2=g_patientHead;
            while (p2 && p2->patientId!=id) p2=p2->next;
            if (!p2) { printf("[-] Not found.\n"); continue; }
            printf("\n====== Cost Summary: %s (ID %d) ======\n", p2->name, p2->patientId);
            float regCost=0, conCost=0, examCost=0, hospCost=0;
            VisitRecord *vr=p2->records;
            while (vr) {
                if (vr->recordType==1) regCost  += vr->cost;
                else if (vr->recordType==2) conCost += vr->cost;
                else if (vr->recordType==3) examCost+= vr->cost;
                else if (vr->recordType==4) hospCost+= vr->cost;
                vr=vr->next;
            }
            float drugCost=0;
            Prescription *pr=g_presHead;
            while (pr) {
                if (pr->patientId==id) drugCost+=pr->totalCost;
                pr=pr->next;
            }
            printf("  Registration  : %8.2f\n", regCost);
            printf("  Consultation  : %8.2f\n", conCost);
            printf("  Examination   : %8.2f\n", examCost);
            printf("  Inpatient care: %8.2f\n", hospCost);
            printf("  Drug/Prescr.  : %8.2f\n", drugCost);
            printf("  ─────────────────────────\n");
            printf("  TOTAL         : %8.2f\n", p2->totalCost + drugCost);
        }
    }
}

/* ===== ADMIN: Doctor CRUD ===== */
static void admin_doctor_management(void) {
    while (1) {
        printf("\n============================================\n");
        printf("         ADMIN — DOCTOR MANAGEMENT\n");
        printf("============================================\n");
        printf("  1. List All Doctors\n");
        printf("  2. Add New Doctor\n");
        printf("  3. Search Doctor by ID\n");
        printf("  4. Delete Doctor by ID\n");
        printf("  0. Back\n");
        int ch = get_int("Select: ", 0, 4);
        if (ch == 0) break;

        if (ch == 1) {
            Doctor *d = g_doctorHead; int n=0;
            printf("\n%-6s %-20s %-12s %-16s %-8s\n","ID","Name","Dept","Title","Patients");
            printf("---------------------------------------------------------------\n");
            while (d) {
                printf("%-6d %-20s %-12s %-16s %-8d\n",
                       d->doctorId, d->name, d->department, d->title, d->patientCount);
                d=d->next; n++;
            }
            printf("Total: %d\n", n);

        } else if (ch == 2) {
            Doctor *nd = (Doctor*)calloc(1,sizeof(Doctor));
            nd->doctorId = g_nextDoctorId++;
            get_str("Name       : ", nd->name, NAME_LEN);
            get_str("Department : ", nd->department, DEPT_LEN);
            get_str("Title      : ", nd->title, TITLE_LEN);
            nd->patientCount=0; nd->next=NULL;
            if (!g_doctorHead){g_doctorHead=nd;}
            else{Doctor *t=g_doctorHead;while(t->next)t=t->next;t->next=nd;}
            printf("[+] Doctor added: ID=%d\n", nd->doctorId);

        } else if (ch == 3) {
            int id = get_int("Doctor ID: ", 1, 999999);
            Doctor *d=g_doctorHead;
            while(d && d->doctorId!=id) d=d->next;
            if(d) printf("\nID:%d  Name:%s  Dept:%s  Title:%s  Patients:%d\n",
                         d->doctorId,d->name,d->department,d->title,d->patientCount);
            else printf("[-] Not found.\n");

        } else if (ch == 4) {
            int id = get_int("Doctor ID to delete: ", 1, 999999);
            Doctor *d=g_doctorHead,*prev=NULL;
            while(d && d->doctorId!=id){prev=d;d=d->next;}
            if(d){if(prev)prev->next=d->next;else g_doctorHead=d->next;free(d);printf("[+] Deleted.\n");}
            else printf("[-] Not found.\n");
        }
    }
}

/* ===== ADMIN: Medicine CRUD ===== */
static void admin_medicine_management(void) {
    while (1) {
        printf("\n============================================\n");
        printf("         ADMIN — MEDICINE MANAGEMENT\n");
        printf("============================================\n");
        printf("  1. %s\n", L("List All Medicines (with low-stock alerts)","查看所有药品（含预警）"));
        printf("  2. %s\n", L("Add New Medicine","新增药品"));
        printf("  3. %s\n", L("Search Medicine by ID","按编号查询"));
        printf("  4. %s\n", L("Delete Medicine by ID","按编号删除"));
        printf("  5. %s\n", L("Sort by Price (ascending)","按价格排序（升序）"));
        printf("  6. %s\n", L("Sort by Stock (ascending)","按库存排序（升序）"));
        printf("  0. %s\n", L("Back","返回"));
        int ch = get_int(L("Select: ","请选择："), 0, 6);
        if (ch == 0) break;

        if (ch == 1) {
            Medicine *m=g_medicineHead; int n=0, warns=0;
            printf("\n%-6s %-20s %-20s %-8s %-6s %s\n","ID","Name","Generic","Price","Stock","Status");
            printf("---------------------------------------------------------------------\n");
            while(m){
                const char *status = (m->stock <= m->warningLine) ? "[LOW!]" : "OK";
                if (m->stock <= m->warningLine) warns++;
                printf("%-6d %-20s %-20s %-8.2f %-6d %s\n",
                       m->medicineId,m->name,m->genericName,m->price,m->stock,status);
                m=m->next; n++;
            }
            printf(L("Total: %d medicines | Low-stock alerts: %d\n","共 %d 种药品 | 低库存预警：%d 种\n"),n,warns);
        } else if (ch == 5) {
            medicine_sort_by_price(&g_medicineHead);
            printf(L("[+] Sorted by price (ascending).\n","[+] 已按价格升序排序。\n"));
        } else if (ch == 6) {
            medicine_sort_by_stock(&g_medicineHead);
            printf(L("[+] Sorted by stock (ascending).\n","[+] 已按库存升序排序。\n"));

        } else if (ch == 2) {
            Medicine *nm=(Medicine*)calloc(1,sizeof(Medicine));
            nm->medicineId=g_nextMedicineId++;
            get_str("Name       : ", nm->name, MED_NAME_LEN);
            get_str("Brand Name : ", nm->brandName, MED_NAME_LEN);
            get_str("Generic    : ", nm->genericName, MED_NAME_LEN);
            get_str("Alias      : ", nm->alias, MED_NAME_LEN);
            printf("Price      : "); scanf("%f",&nm->price); flush();
            nm->stock   = get_int("Stock      : ", 0, 99999);
            nm->warningLine = get_int("Warning Line: ", 0, 9999);
            nm->next=NULL;
            if(!g_medicineHead){g_medicineHead=nm;}
            else{Medicine *t=g_medicineHead;while(t->next)t=t->next;t->next=nm;}
            printf("[+] Medicine added: ID=%d\n",nm->medicineId);

        } else if (ch == 3) {
            int id=get_int("Medicine ID: ",1,999999);
            Medicine *m=g_medicineHead;
            while(m && m->medicineId!=id) m=m->next;
            if(m) printf("\nID:%d Name:%s Brand:%s Generic:%s Alias:%s Price:%.2f Stock:%d Warn:%d\n",
                         m->medicineId,m->name,m->brandName,m->genericName,m->alias,m->price,m->stock,m->warningLine);
            else printf("[-] Not found.\n");

        } else if (ch == 4) {
            int id=get_int("Medicine ID to delete: ",1,999999);
            Medicine *m=g_medicineHead,*prev=NULL;
            while(m && m->medicineId!=id){prev=m;m=m->next;}
            if(m){if(prev)prev->next=m->next;else g_medicineHead=m->next;free(m);printf("[+] Deleted.\n");}
            else printf("[-] Not found.\n");
        }
    }
}

/* ===== ADMIN Dashboard ===== */
static void dashboard_admin(User *user) {
    while (1) {
        printf("\n╔══════════════════════════════════════════╗\n");
        printf("║  %s — %s\n", L("ADMIN DASHBOARD","管理员控制台"), user->username);
        printf("╠══════════════════════════════════════════╣\n");
        printf("║  1.  %s\n", L("Patient Management (CRUD)","患者管理（增删改查）"));
        printf("║  2.  %s\n", L("Doctor Management (CRUD)","医生管理（增删改查）"));
        printf("║  3.  %s\n", L("Medicine Management (CRUD)","药品管理（增删改查）"));
        printf("║  4.  %s\n", L("User Management (Approve/Delete)","用户管理"));
        printf("║  5.  %s\n", L("Module A — Registration & Records","A模块 — 挂号与病历"));
        printf("║  6.  %s\n", L("Module B — Queue & Consultation","B模块 — 候诊与就诊"));
        printf("║  7.  %s\n", L("Module C — Pharmacy & Prescription","C模块 — 药房与处方"));
        printf("║  8.  %s\n", L("Module D — Inpatient & Reports","D模块 — 住院与报告"));
        printf("║  0.  %s\n", L("Logout","退出登录"));
        printf("╚══════════════════════════════════════════╝\n");
        int ch = get_int(L("Select: ","请选择："), 0, 8);
        if (ch==0) { printf("%s\n", L("Logging out...","正在退出登录...") ); break; }
        switch(ch) {
            case 1: admin_patient_management();  break;
            case 2: admin_doctor_management();   break;
            case 3: admin_medicine_management(); break;
            case 4: admin_user_management();     break;
            case 5: A_entry(); break;
            case 6: B_entry(); break;
            case 7: C_entry(); break;
            case 8: D_entry(); break;
        }
    }
}

/* ===== RECEPTIONIST Dashboard ===== */
static void dashboard_receptionist(User *user) {
    while (1) {
        printf("\n╔══════════════════════════════════════════╗\n");
        printf("║  %s — %s\n", L("RECEPTIONIST DASHBOARD","接诊员控制台"), user->username);
        printf("╠══════════════════════════════════════════╣\n");
        printf("║  1.  %s\n", L("Module A — Patient Registration","A模块 — 患者挂号"));
        printf("║  2.  %s\n", L("Module B — Queue Management","B模块 — 候诊管理"));
        printf("║  3.  %s\n", L("View Patient List","查看患者列表"));
        printf("║  0.  %s\n", L("Logout","退出登录"));
        printf("╚══════════════════════════════════════════╝\n");
        int ch = get_int(L("Select: ","请选择："), 0, 3);
        if (ch==0) { printf("%s\n", L("Logging out...","正在退出登录...")); break; }
        switch(ch) {
            case 1: A_entry(); break;
            case 2: B_entry(); break;
            case 3: {
                Patient *p=g_patientHead; int n=0;
                printf("\n%-6s %-20s %-4s %-6s %-10s\n","ID","Name","Age","Gender","Dept");
                printf("----------------------------------------------------\n");
                while(p){
                    printf("%-6d %-20s %-4d %-6s %-10s\n",
                           p->patientId,p->name,p->age,p->gender,p->department);
                    p=p->next; n++;
                }
                printf("Total: %d\n",n);
            } break;
        }
    }
}

/* ===== DOCTOR Dashboard ===== */
static void dashboard_doctor(User *user) {
    while (1) {
        printf("\n╔══════════════════════════════════════════╗\n");
        printf("║  %s — %s\n", L("DOCTOR DASHBOARD","医生控制台"), user->username);
        printf("╠══════════════════════════════════════════╣\n");
        printf("║  1.  %s\n", L("Module B — Queue & Consultation","B模块 — 候诊与就诊"));
        printf("║  2.  %s\n", L("Module C — Prescription & Pharmacy","C模块 — 处方与药房"));
        printf("║  3.  %s\n", L("Module D — Inpatient & Reports","D模块 — 住院与报告"));
        printf("║  4.  %s\n", L("View All Patients","查看所有患者"));
        printf("║  5.  %s\n", L("Search Patient by ID","按编号查询患者"));
        printf("║  0.  %s\n", L("Logout","退出登录"));
        printf("╚══════════════════════════════════════════╝\n");
        int ch = get_int(L("Select: ","请选择："), 0, 5);
        if (ch==0) { printf("%s\n", L("Logging out...","正在退出登录...")); break; }
        switch(ch) {
            case 1: B_entry(); break;
            case 2: C_entry(); break;
            case 3: D_entry(); break;
            case 4: {
                Patient *p=g_patientHead; int n=0;
                printf("\n%-6s %-20s %-4s %-6s %-10s %-8s\n","ID","Name","Age","Gender","Dept","Hosp.");
                printf("------------------------------------------------------------\n");
                while(p){
                    printf("%-6d %-20s %-4d %-6s %-10s %-8s\n",
                           p->patientId,p->name,p->age,p->gender,p->department,
                           p->isHospitalized?"YES":"NO");
                    p=p->next; n++;
                }
                printf("Total: %d\n",n);
            } break;
            case 5: {
                int id=get_int("Patient ID: ",1,999999);
                Patient *p=g_patientHead;
                while(p && p->patientId!=id) p=p->next;
                if(p) printf("\nID:%d Name:%s Age:%d Gender:%s Dept:%s Hosp:%s Bed:%d\n",
                             p->patientId,p->name,p->age,p->gender,p->department,
                             p->isHospitalized?"YES":"NO",p->bedId);
                else printf("[-] Not found.\n");
            } break;
        }
    }
}

/* ===== PATIENT Dashboard ===== */
static void dashboard_patient(User *user) {
    while (1) {
        printf("\n╔══════════════════════════════════════════╗\n");
        printf("║  %s — %s\n", L("PATIENT DASHBOARD","患者控制台"), user->username);
        printf("╠══════════════════════════════════════════╣\n");
        printf("║  1.  %s\n", L("View My Records (Module A)","查看我的病历（A模块）"));
        printf("║  2.  %s\n", L("View Available Medicines","查看可用药品"));
        printf("║  0.  %s\n", L("Logout","退出登录"));
        printf("╚══════════════════════════════════════════╝\n");
        int ch = get_int(L("Select: ","请选择："), 0, 2);
        if (ch==0) { printf("%s\n", L("Logging out...","正在退出登录...")); break; }
        switch(ch) {
            case 1: A_entry(); break;
            case 2: {
                Medicine *m=g_medicineHead; int n=0;
                printf("\n%-6s %-20s %-20s %-8s %-6s\n","ID","Name","Generic","Price","Stock");
                printf("------------------------------------------------------------\n");
                while(m){
                    printf("%-6d %-20s %-20s %-8.2f %-6d\n",
                           m->medicineId,m->name,m->genericName,m->price,m->stock);
                    m=m->next; n++;
                }
                printf("Total: %d medicines\n",n);
            } break;
        }
    }
}

/* ===== Main ===== */
int main(void) {
    /* Load users from file */
    auth_init();

    /* Initialize all global data (patients, doctors, medicines, beds) */
    init_system_globals();
    init_hospital_globals();

    while (1) {
        printf("\n========================================\n");
        printf("   %s\n", L("HOSPITAL MANAGEMENT SYSTEM","医院管理系统"));
        printf("========================================\n");
        printf("  1. %s\n", L("Login","登录"));
        printf("  2. %s\n", L("Register (requires admin approval)","注册（需管理员审批）"));
        printf("  9. %s  [%s]\n", L("Change Language","切换语言"), g_lang?"English/英文":"中文/Chinese");
        printf("  0. %s\n", L("Exit","退出"));
        printf("========================================\n");

        int choice = get_int(L("Select: ","请选择："), 0, 9);
        if (choice == 0) { printf("%s\n", L("Goodbye!","再见！")); break; }
        if (choice == 9) {
            g_lang = !g_lang;
            printf(g_lang ? "\n[+] 语言已切换为：中文\n" : "\n[+] Language set to: English\n");
            continue;
        }

        if (choice == 1) {
            char username[MAX_USERNAME_LEN], password[MAX_PASSWORD_LEN];
            printf("%s", L("Username: ","用户名：")); scanf("%49s",username); flush();
            printf("%s", L("Password: ","密码："));  scanf("%63s",password); flush();

            User *user = auth_login(username, password);
            if (!user) {
                printf("\n[-] %s\n", L("Login failed! Check credentials or account not yet approved.","登录失败！请检查账户或等待管理员审批。"));
                continue;
            }
            printf("\n[+] %s, %s! %s: %s\n",
                   L("Welcome","欢迎"), user->full_name,
                   L("Role","角色"), auth_role_to_string(user->role));

            switch (user->role) {
                case ROLE_ADMIN:        dashboard_admin(user);        break;
                case ROLE_RECEPTIONIST: dashboard_receptionist(user); break;
                case ROLE_DOCTOR:       dashboard_doctor(user);       break;
                case ROLE_PATIENT:      dashboard_patient(user);      break;
                default: printf("[-] %s\n", L("Unknown role.","未知角色。")); break;
            }

        } else if (choice == 2) {
            do_register_public();
        }
    }

    auth_cleanup();
    free_all_lists();
    printf(L("[+] Memory released. Goodbye!\n","[+] 内存已释放。再见！\n"));
    return EXIT_SUCCESS;
}
