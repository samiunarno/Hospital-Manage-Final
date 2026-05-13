#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "../include/common.h"

// Note: Removed local struct definitions (Registration, Medicine, PrescriptionItem, Prescription)
// since they are unified in common.h


/* ===== prescription.h function declarations ===== */
int prescription_check_visit_status(int patientId);
int prescription_create_for_called_patient(int patientId);
int prescription_add_item_by_name(Medicine* medHead, int presId, char* medName, int num);
void prescription_display_one(int presId);
void prescription_display_all(void);
void prescription_display_by_patient(int patientId);
void prescription_free_all(void);

/* ===== pharmacy function declarations ===== */
void clearInputBuffer(void);
Medicine* medicine_in(Medicine** head, char* name, char* brandName, char* genericName, char* alias, int price, int stock, int warningLine);
Medicine* medicine_select(Medicine* head, char* name);
void medicine_delete(Medicine** head, char* name);
void medicine_menu(Medicine** head);
void medicine_stock(Medicine* head);
void medicine_print(Medicine* head);
void saveToFile(Medicine *head);
void loadFromFile(Medicine **head);

/* ====================== prescription.c ====================== */

/* Validate patient is in consultation (status=2) */
int prescription_check_visit_status(int patientId)
{
    /* 1. Basic ID validation */
    if(patientId < 1001)
    {
        printf("--------------------------------------------------\n");
        printf("Invalid Patient ID, prescription denied!\n");
        return 0;
    }

    /* 2. Search registration list for consultation status */
    Registration* reg = g_regHead;
    int isVisiting = 0;
    while(reg != NULL)
    {
        if(reg->patientId == patientId && reg->status == 2)
        {
            isVisiting = 1;
            break;
        }
        reg = reg->next;
    }

    /* 3. Status check */
    if(!isVisiting)
    {
        printf("--------------------------------------------------\n");
        printf("Patient not in consultation, prescription denied!\n");
        return 0;
    }

    printf("--------------------------------------------------\n");
    printf("Patient consultation verified, prescription allowed\n");
    return 1;
}

/* Functions below retain original logic */
int prescription_create_for_called_patient(int patientId)
{
    if(!prescription_check_visit_status(patientId))
        return 0;

    Prescription* newPres = (Prescription*)malloc(sizeof(Prescription));
    if(!newPres)
    {
        printf("Memory allocation failed!\n");
        return 0;
    }
    newPres->prescriptionId = g_nextPrescriptionId++;
    newPres->patientId = patientId;
    newPres->totalCost = 0;
    newPres->items = NULL;
    newPres->next = NULL;

    /* Append to list */
    if(g_presHead == NULL)
        g_presHead = newPres;
    else
    {
        Prescription* p = g_presHead;
        while(p->next) p = p->next;
        p->next = newPres;
    }
    printf("Success! Prescription ID: %d\n", newPres->prescriptionId);
    return 1;
}

int prescription_add_item_by_name(Medicine* medHead, int presId, char* medName, int num)
{
    if(num <= 0)
    {
        printf("Invalid input! Quantity must be > 0\n");
        return 0;
    }

    /* Find prescription */
    Prescription* p = g_presHead;
    while(p && p->prescriptionId != presId)
        p = p->next;
    if(!p)
    {
        printf("Prescription not found!\n");
        return 0;
    }

    /* Find medicine */
    Medicine* med = medicine_select(medHead, medName);
    if(!med) return 0;

    /* Stock check */
    if(med->stock < num)
    {
        printf("Insufficient stock！\n");
        return 0;
    }

    /* Create item */
    PrescriptionItem* item = (PrescriptionItem*)malloc(sizeof(PrescriptionItem));
    if(!item)
    {
        printf("Memory allocation failed!\n");
        return 0;
    }
    strcpy(item->medicineName, medName);
    item->quantity = num;
    item->itemCost = med->price * num;
    item->next = NULL;

    /* Append item to list */
    if(!p->items)
        p->items = item;
    else
    {
        PrescriptionItem* q = p->items;
        while(q->next) q = q->next;
        q->next = item;
    }

    /* Add to total cost */
    p->totalCost += item->itemCost;
    /* Deduct stock */
    med->stock -= num;

    printf("Success! Medicine added and stock deducted.\n");
    return 1;
}

void prescription_display_one(int presId)
{
    Prescription* p = g_presHead;
    while(p && p->prescriptionId != presId)
        p = p->next;
    if(!p)
    {
        printf("No matching record found!\n");
        return;
    }

    printf("---------------- Prescription Info ----------------\n");
    printf("Prescription ID: %d\n", p->prescriptionId);
    printf("Patient ID: %d\n", p->patientId);
    printf("Total Cost: %.2f\n", p->totalCost);
    printf("----------------------------------------\n");
    printf("Medicine Name\tQty\tItem Cost\n");

    PrescriptionItem* item = p->items;
    if(!item)
    {
        printf("No medicine items\n");
        return;
    }
    while(item)
    {
        printf("%s\t%d\t%.2f\n",
               item->medicineName,
               item->quantity,
               item->itemCost);
        item = item->next;
    }
    printf("--------------------------------------------------\n");
}

void prescription_display_all(void)
{
    if(!g_presHead)
    {
        printf("No prescriptions found!\n");
        return;
    }
    Prescription* p = g_presHead;
    while(p)
    {
        prescription_display_one(p->prescriptionId);
        p = p->next;
    }
}

void prescription_display_by_patient(int patientId)
{
    int flag = 0;
    Prescription* p = g_presHead;
    while(p)
    {
        if(p->patientId == patientId)
        {
            prescription_display_one(p->prescriptionId);
            flag = 1;
        }
        p = p->next;
    }
    if(!flag)
        printf("No prescriptions found for this patient!\n");
}

void prescription_free_all(void)
{
    Prescription* p = g_presHead;
    while(p)
    {
        Prescription* t1 = p;
        p = p->next;

        PrescriptionItem* item = t1->items;
        while(item)
        {
            PrescriptionItem* t2 = item;
            item = item->next;
            free(t2);
        }
        free(t1);
    }
    g_presHead = NULL;
}

/* ====================== pharmacy(1).c ====================== */

void clearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

Medicine* medicine_in(Medicine** head, char* name, char* brandName, char* genericName, char* alias, int price, int stock, int warningLine) {
    Medicine* medicine_new = (Medicine*)malloc(sizeof(Medicine));
    if (medicine_new == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    medicine_new->medicineId = g_nextMedicineId++;
    strncpy(medicine_new->name, name ? name : "", MED_NAME_LEN - 1);
    medicine_new->name[MED_NAME_LEN - 1] = '\0';
    strncpy(medicine_new->brandName, brandName ? brandName : "", MED_NAME_LEN - 1);
    medicine_new->brandName[MED_NAME_LEN - 1] = '\0';
    strncpy(medicine_new->genericName, genericName ? genericName : "", MED_NAME_LEN - 1);
    medicine_new->genericName[MED_NAME_LEN - 1] = '\0';
    strncpy(medicine_new->alias, alias ? alias : "", MED_NAME_LEN - 1);
    medicine_new->alias[MED_NAME_LEN - 1] = '\0';
    medicine_new->price = price;
    medicine_new->stock = stock;
    medicine_new->warningLine = warningLine;
    medicine_new->next = NULL;

    if (*head == NULL) {
        *head = medicine_new;
        return medicine_new;
    }
    Medicine* last = *head;
    while (last->next != NULL) {
        last = last->next;
    }
    last->next = medicine_new;
    return medicine_new;
}

Medicine* medicine_select(Medicine* head, char* name) {
    Medicine* last = head;
    while (last != NULL) {
        if (strcmp(last->name, name) == 0) {
            return last;
        }
        last = last->next;
    }
    printf("Medicine not found!\n");
    return NULL;
}

void medicine_delete(Medicine** head, char* name) {
    if (*head == NULL) {
        printf("Medicine list empty, delete failed!\n");
        return;
    }
    Medicine* last = *head;
    Medicine* pre = NULL;
    while (last != NULL) {
        if (strcmp(last->name, name) == 0) {
            free(last->name);
            if (pre == NULL) {
                *head = last->next;
            } else {
                pre->next = last->next;
            }
            free(last);
            printf("Medicine deleted: %s\n", name);
            return;
        }
        pre = last;
        last = last->next;
    }
    printf("Medicine not found: %s\n", name);
}

/* ====================== Fix: parameter changed to double pointer ====================== */
void medicine_stock(Medicine* head) {
    int num;
    char name[50];
    int add;
    int out;
    printf("--- Stock Operations ---\n1. Stock In\n2. Stock Out\n0. Back\nSelect: ");
    scanf("%d", &num);
    clearInputBuffer();

    if (num == 1) {
        printf("Enter MedicineName: ");
        scanf("%s", name);
        Medicine* p = medicine_select(head, name);
        if (p == NULL) {
            printf("Medicine not found!\n");
            return;
        }
        printf("Enter stock-in quantity: ");
        a:
        while (scanf("%d", &add) != 1) {
            clearInputBuffer();
            printf("Invalid input, re-enter: ");
        }
        if (add <= 0) {
            printf("Quantity cannot be negative! Re-enter: ");
            goto a;
        }
        p->stock += add;
        printf("Stock added successfully!\n");
    } else if (num == 2) {
        printf("Enter MedicineName: ");
        scanf("%s", name);
        Medicine* p = medicine_select(head, name);
        if (p == NULL) {
            printf("Medicine not found!\n");
            return;
        }
        printf("Enter stock-out quantity: ");
        b:
        while (scanf("%d", &out) != 1) {
            clearInputBuffer();
            printf("Invalid input, re-enter: ");
        }
        if (p->stock < out) {
            printf("Stock cannot be negative! Re-enter: ");
            goto b;
        }
        p->stock -= out;
        printf("Stock updated successfully!\n");
    } else if (num == 0) {
        /* Fixed: no recursion, return directly */
        return;
    } else {
        printf("Invalid operation!\n");
    }
}

/* ====================== Fix: double pointer ====================== */
void medicine_menu(Medicine** head) {
    int num;
    char name1[50] = {0};
    char brandName[50] = {0};
    char genericName[50] = {0};
    char alias[50] = {0};
    char name2[50] = {0};
    int price;
    int stock;
    int warningLine;

    printf("--- Medicine Operations ---\n1. Add Medicine\n2. Delete Medicine\n3. Stock Operations\n0. Back\nSelect: ");
    scanf("%d", &num);
    clearInputBuffer();

    if (num == 1) {
        printf("MedicineName: ");
        scanf("%s", name1);
        printf("BrandName: ");
        scanf("%s", brandName);
        printf("Generic Name: ");
        scanf("%s", genericName);
        printf("Alias: ");
        scanf("%s", alias);
        printf("MedicinePrice: ");
        scanf("%d", &price);
        printf("MedicineStock: ");
        scanf("%d", &stock);
        printf("Warning Line: ");
        scanf("%d", &warningLine);
        medicine_in(head, name1, brandName, genericName, alias, price, stock, warningLine);
        printf("Added successfully!\n");
    } else if (num == 2) {
        printf("Enter MedicineName: ");
        scanf("%s", name2);
        medicine_delete(head, name2);
    } else if (num == 3) {
        medicine_stock(*head);
    } else if (num != 0) {
        printf("Invalid option\n");
    }
}

void medicine_print(Medicine* head) {
    Medicine* last = head;
    int i = 1;
    while (last != NULL) {
        printf("===== Medicine %d =====\nName: %s\nBrand: %s\nGeneric: %s\nAlias: %s\nPrice: %.2f\nStock: %d\nWarning: %d\n",
               i, last->name, last->brandName, last->genericName, last->alias, last->price, last->stock, last->warningLine);
        i++;
        last = last->next;
    }
}

/* ====================== File I/O ====================== */
void saveToFile(Medicine *head) {
    FILE *fp = fopen("medicine.txt", "w");
    if (fp == NULL) {
        printf("Cannot open file for saving\n");
        return;
    }
    Medicine *p = head;
    while (p != NULL) {
        fprintf(fp, "%s %s %s %s %.2f %d %d\n", p->name, p->brandName, p->genericName, p->alias, p->price, p->stock, p->warningLine);
        p = p->next;
    }
    fclose(fp);
    printf("All medicines saved to medicine.txt\n");
}

void loadFromFile(Medicine **head) {
    FILE *fp = fopen("medicine.txt", "r");
    if (fp == NULL) {
        printf("No historical data, creating empty list\n");
        return;
    }
    char name[100], brandName[100], genericName[100], alias[100];
    float price;
    int stock, warningLine;
    while (fscanf(fp, "%s %s %s %s %f %d %d", name, brandName, genericName, alias, &price, &stock, &warningLine) != EOF) {
        medicine_in(head, name, brandName, genericName, alias, price, stock, warningLine);
    }
    fclose(fp);
    printf("Medicine data loaded from file\n");
}

/* ====================== pharmacy entry function ====================== */
#ifdef C_MODULE_STANDALONE
int main() {
    Medicine *head = NULL;

    loadFromFile(&head);

    int op;
    while (1) {
        printf("\n===== Main Menu =====\n");
        printf("1. Medicine Operations\n2. Print Medicines\n3. Save & Exit\nSelect: ");
        scanf("%d", &op);
        clearInputBuffer();

        if (op == 1) {
            medicine_menu(&head);
        } else if (op == 2) {
            medicine_print(head);
        } else if (op == 3) {
            saveToFile(head);
            break;
        } else {
            printf("Invalid option\n");
        }
    }

    return 0;
}
#endif


static void c_prepare_demo_registration(void) {
    if (g_regHead != NULL) return;
    Registration* r = (Registration*)malloc(sizeof(Registration));
    if (!r) return;
    r->registerId = 7001;
    r->patientId = 1001;
    r->doctorId = 2001;
    r->status = 2;
    r->next = NULL;
    g_regHead = r;
}

int C_entry(void) {
    Medicine *head = g_medicineHead;
    int op;

    while (1) {
        printf("\n===== Module C — Pharmacy Menu =====\n");
        printf("1. Medicine Operations\n");
        printf("2. Print Medicines\n");
        printf("3. Create Prescription (Patient 1001)\n");
        printf("4. Add Medicine to Prescription\n");
        printf("5. Display All Prescriptions\n");
        printf("0. Back\nSelect: ");
        if (scanf("%d", &op) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (op == 1) {
            medicine_menu(&head);
        } else if (op == 2) {
            medicine_print(head);
        } else if (op == 3) {
            prescription_create_for_called_patient(1001);
        } else if (op == 4) {
            int presId, num;
            char medName[100];
            printf("Enter Prescription ID: ");
            scanf("%d", &presId);
            clearInputBuffer();
            printf("Enter MedicineName: ");
            scanf("%99s", medName);
            clearInputBuffer();
            printf("Enter Quantity: ");
            scanf("%d", &num);
            clearInputBuffer();
            prescription_add_item_by_name(head, presId, medName, num);
        } else if (op == 5) {
            prescription_display_all();
        } else if (op == 0) {
            saveToFile(head);
            return 0;
        } else {
            printf("Invalid option\n");
        }
    }
}
