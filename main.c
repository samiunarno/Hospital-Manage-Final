/**
 * @file main.c
 * @brief 四人统一基础版项目 - 主菜单入口
 * 
 * 这是一个模块化医院管理系统的主入口文件。
 * 将四个子系统 (A, B, C, D) 整合为一个统一的应用程序。
 * 
 * 编译方式：
 *   gcc -O2 -Wall -Wextra -o hospital main.c A.c B.c C.c D.c utils.c
 * 
 * @note hospital.c 是 D 模块的早期副本，请勿一起编译。
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>

/* 四个模块的入口函数声明 */
extern int A_entry(void);   /**< A 模块：患者管理 / 挂号 / 初始化 */
extern int B_entry(void);   /**< B 模块：候诊队列 / 叫号 */
extern int C_entry(void);   /**< C 模块：处方 / 药房 */
extern int D_entry(void);   /**< D 模块：住院管理 / 统计报表 */

/**
 * @brief 清空标准输入缓冲区，防止非法输入导致的无限循环
 */
static void flush_input_buffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* 丢弃字符直到遇到换行符或文件结束符 */
    }
}

/**
 * @brief 显示主菜单界面
 */
static void display_menu(void) {
    printf("\n========================================\n");
    printf("      四人统一基础版项目（拼装版）\n");
    printf("========================================\n");
    printf("1. A 模块（患者/挂号/初始化）\n");
    printf("2. B 模块（候诊/叫号）\n");
    printf("3. C 模块（处方/药房）\n");
    printf("4. D 模块（住院/报表）\n");
    printf("0. 退出\n");
    printf("========================================\n");
    printf("请选择：");
}

int main(void) {
    /* 初始化本地化设置，支持多字节字符（例如 UTF-8 中文） */
    setlocale(LC_ALL, "");

    bool is_running = true;
    while (is_running) {
        display_menu();

        int choice = -1;
        if (scanf("%d", &choice) != 1) {
            printf("\n[!] 错误：输入无效。请输入数字。\n");
            flush_input_buffer();
            continue;
        }
        flush_input_buffer(); /* 捕获任何多余的尾部字符 */

        switch (choice) {
            case 1:
                printf("\n--- 正在启动 A 模块 ---\n");
                A_entry();
                break;
            case 2:
                printf("\n--- 正在启动 B 模块 ---\n");
                B_entry();
                break;
            case 3:
                printf("\n--- 正在启动 C 模块 ---\n");
                C_entry();
                break;
            case 4:
                printf("\n--- 正在启动 D 模块 ---\n");
                D_entry();
                break;
            case 0:
                printf("\n系统已退出。\n");
                is_running = false;
                break;
            default:
                printf("\n[!] 错误：无效选项（%d）。请重试。\n", choice);
                break;
        }
    }

    return EXIT_SUCCESS;
}
