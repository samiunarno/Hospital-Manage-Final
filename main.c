
#include <stdio.h>
#include <locale.h>

int A_entry(void);
int B_entry(void);
int C_entry(void);
int D_entry(void);

int main(void) {
    setlocale(LC_ALL, "");
    while (1) {
        int choice;
        printf("\n========================================\n");
        printf("      四人统一基础版项目（拼装版）\n");
        printf("========================================\n");
        printf("1. A 模块（患者/挂号/初始化）\n");
        printf("2. B 模块（候诊/叫号）\n");
        printf("3. C 模块（处方/药房）\n");
        printf("4. D 模块（住院/报表）\n");
        printf("0. 退出\n");
        printf("请选择：");
        if (scanf("%d", &choice) != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            continue;
        }
        while (getchar() != '\n' && !feof(stdin)) {}

        switch (choice) {
            case 1: A_entry(); break;
            case 2: B_entry(); break;
            case 3: C_entry(); break;
            case 4: D_entry(); break;
            case 0: return 0;
            default: printf("无效选项\n");
        }
    }
}
