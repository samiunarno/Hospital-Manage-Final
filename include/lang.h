#ifndef LANG_H
#define LANG_H

/*
 * lang.h — Bilingual string system (English / Chinese)
 * Usage:  L("English text", "中文文字")
 * g_lang: 0 = English,  1 = Chinese (Simplified)
 */

extern int g_lang;   /* defined in main.c */

#define L(en, zh)   (g_lang == 0 ? (en) : (zh))

/* ───────────────────────────────── Common ─────────────────────────── */
#define STR_BACK        L("0. Back",                     "0. 返回")
#define STR_LOGOUT      L("0. Logout",                   "0. 退出登录")
#define STR_SELECT      L("Select: ",                    "请选择：")
#define STR_NOT_FOUND   L("Not found!",                  "未找到！")
#define STR_OP_OK       L("Operation successful.",       "操作成功。")
#define STR_OP_FAIL     L("Operation failed!",           "操作失败！")
#define STR_ADDED       L("Added successfully.",         "添加成功。")
#define STR_UPDATED     L("Updated successfully.",       "修改成功。")
#define STR_DELETED     L("Deleted successfully.",       "删除成功。")
#define STR_INVALID     L("[!] Invalid option.",         "[!] 无效选项。")
#define STR_ENTER_ID    L("Enter ID: ",                  "输入编号：")

/* ───────────────────────────────── System banner ───────────────────── */
#define STR_TITLE       L("HOSPITAL MANAGEMENT SYSTEM", "医院管理系统")
#define STR_INIT_MSG    L("Initializing Hospital System (Full Environment)", \
                          "正在初始化医院系统（满载测试环境）")
#define STR_INIT_OK     L("Hospital system initialized!", "医院系统初始化成功！")
#define STR_INIT_DEPT   L("  - 5 major departments",     "  - 5 个主要科室")
#define STR_INIT_WARD   L("  - 9 wards (General/ICU/Private)", "  - 9 个病房（普通/重症/单人）")
#define STR_INIT_BED    L("  - 70 total beds",           "  - 70 张总床位")

/* ───────────────────────────────── Login / Register ────────────────── */
#define STR_LOGIN       L("Login",                       "登录")
#define STR_REGISTER    L("Register (requires admin approval)", "注册（需管理员审批）")
#define STR_EXIT        L("Exit",                        "退出")
#define STR_USERNAME    L("Username: ",                  "用户名：")
#define STR_PASSWORD    L("Password: ",                  "密码：")
#define STR_WELCOME     L("Welcome",                     "欢迎")
#define STR_ROLE        L("Role",                        "角色")
#define STR_LOGIN_FAIL  L("Login failed! Check credentials or account pending approval.", \
                          "登录失败！请检查账户或等待管理员审批。")
#define STR_REG_OK      L("Registered! Waiting for admin approval.", \
                          "注册成功！等待管理员审批后可登录。")
#define STR_REG_FAIL    L("Registration failed: username/NID already exists.", \
                          "注册失败：用户名或身份证号已存在。")
#define STR_FULL_NAME   L("Full Name: ",                 "真实姓名：")
#define STR_NATIONAL_ID L("National ID: ",               "身份证号：")
#define STR_ROLE_SEL    L("Role (1=Receptionist 2=Doctor 3=Patient): ", \
                          "角色（1=接诊员 2=医生 3=患者）：")
#define STR_LOGGING_OUT L("Logging out...",              "正在退出登录...")
#define STR_GOODBYE     L("Goodbye!",                    "再见！")
#define STR_CHANGE_LANG L("Change Language / 切换语言",  "Change Language / 切换语言")

/* ───────────────────────────────── Roles ───────────────────────────── */
#define STR_ADMIN       L("Admin",           "管理员")
#define STR_RECEPT      L("Receptionist",    "接诊员")
#define STR_DOCTOR      L("Doctor",          "医生")
#define STR_PATIENT     L("Patient",         "患者")

/* ───────────────────────────────── Admin dashboard ─────────────────── */
#define STR_DASH_ADMIN  L("ADMIN DASHBOARD", "管理员控制台")
#define STR_PAT_MGMT    L("Patient Management (CRUD)",   "患者管理（增删改查）")
#define STR_DOC_MGMT    L("Doctor Management (CRUD)",    "医生管理（增删改查）")
#define STR_MED_MGMT    L("Medicine Management (CRUD)",  "药品管理（增删改查）")
#define STR_USR_MGMT    L("User Management",             "用户管理")
#define STR_MOD_A       L("Module A — Registration & Records",  "A模块 — 挂号与病历")
#define STR_MOD_B       L("Module B — Queue & Consultation",    "B模块 — 候诊与就诊")
#define STR_MOD_C       L("Module C — Pharmacy & Prescription", "C模块 — 药房与处方")
#define STR_MOD_D       L("Module D — Inpatient & Reports",     "D模块 — 住院与报告")

/* ───────────────────────────────── Receptionist dashboard ──────────── */
#define STR_DASH_RECEPT L("RECEPTIONIST DASHBOARD",  "接诊员控制台")
#define STR_VIEW_PAT    L("View Patient List",        "查看患者列表")

/* ───────────────────────────────── Doctor dashboard ────────────────── */
#define STR_DASH_DOC    L("DOCTOR DASHBOARD",         "医生控制台")
#define STR_VIEW_ALL_P  L("View All Patients",        "查看所有患者")
#define STR_SEARCH_P    L("Search Patient by ID",     "按编号查询患者")

/* ───────────────────────────────── Patient dashboard ───────────────── */
#define STR_DASH_PAT    L("PATIENT DASHBOARD",        "患者控制台")
#define STR_MY_RECORDS  L("View My Records (Module A)", "查看我的病历（A模块）")
#define STR_VIEW_MEDS   L("View Available Medicines",  "查看可用药品")

/* ───────────────────────────────── User Management ─────────────────── */
#define STR_USR_PENDING L("View Pending Users",       "查看待审批用户")
#define STR_USR_APPROVE L("Approve User",             "审批用户")
#define STR_USR_REJECT  L("Reject User",              "拒绝用户")
#define STR_USR_VIEW_A  L("View All Approved Users",  "查看已批准用户")
#define STR_USR_CREATE  L("Create New User (auto-approved)", "新建用户（自动批准）")
#define STR_USR_ROLE    L("Update User Role",         "修改用户角色")
#define STR_USR_DELETE  L("Delete User",              "删除用户")
#define STR_PENDING_CNT L("Pending approvals",        "待审批数量")
#define STR_TOTAL_USERS L("Total users",              "总用户数")
#define STR_NO_PENDING  L("(none)",                   "（无）")
#define STR_NO_APPROVED L("No approved users.",       "暂无已批准用户。")
#define STR_APPR_OK     L("Approved",                 "已批准")
#define STR_APPR_FAIL   L("Not found or already approved.", "未找到或已批准。")
#define STR_REJ_OK      L("Rejected & removed",       "已拒绝并删除")
#define STR_REJ_FAIL    L("Not found.",               "未找到。")
#define STR_ROLE_UPD_OK L("Role updated.",            "角色已更新。")
#define STR_ROLE_UPD_F  L("User not found.",          "用户未找到。")
#define STR_DEL_OK      L("User deleted",             "用户已删除")
#define STR_DEL_FAIL    L("Cannot delete: not found or last admin.", \
                          "删除失败：未找到或为最后一个管理员。")
#define STR_ROLE_NEW_SEL L("New Role (0=Admin 1=Receptionist 2=Doctor 3=Patient): ", \
                           "新角色（0=管理员 1=接诊员 2=医生 3=患者）：")
#define STR_ROLE_SEL_A  L("Role (0=Admin 1=Receptionist 2=Doctor 3=Patient): ", \
                          "角色（0=管理员 1=接诊员 2=医生 3=患者）：")

/* ───────────────────────────────── Patient CRUD ─────────────────────── */
#define STR_PAT_TITLE   L("PATIENT MANAGEMENT",       "患者管理")
#define STR_PAT_LIST    L("List All Patients",         "查看所有患者")
#define STR_PAT_ADD     L("Add New Patient",           "新增患者")
#define STR_PAT_SEARCH  L("Search Patient by ID",      "按编号查询患者")
#define STR_PAT_UPDATE  L("Update Patient Info",       "修改患者信息")
#define STR_PAT_DELETE  L("Delete Patient by ID",      "按编号删除患者")
#define STR_ENTER_PID   L("Enter Patient ID: ",        "输入患者编号：")
#define STR_ENTER_DEL   L("Enter Patient ID to delete: ", "输入要删除的患者编号：")
#define STR_COL_ID      L("ID",      "编号")
#define STR_COL_NAME    L("Name",    "姓名")
#define STR_COL_AGE     L("Age",     "年龄")
#define STR_COL_GENDER  L("Gender",  "性别")
#define STR_COL_DEPT    L("Dept",    "科室")
#define STR_COL_HOSP    L("Hosp.",   "住院")
#define STR_COL_COST    L("Cost",    "费用")
#define STR_HOSP_YES    L("YES",     "是")
#define STR_HOSP_NO     L("NO",      "否")
#define STR_NO_PATIENTS L("No patients found.", "暂无患者。")
#define STR_PAT_ADDED   L("Patient added! ID: ", "患者已添加！编号：")
#define STR_PAT_INFO    L("--- Patient Info ---",  "--- 患者信息 ---")
#define STR_ENTER_NAME  L("Name      : ",  "姓名：")
#define STR_ENTER_AGE   L("Age       : ",  "年龄：")
#define STR_ENTER_GENDER L("Gender    : ", "性别：")
#define STR_ENTER_DEPT  L("Department: ",  "科室：")

/* ───────────────────────────────── Doctor CRUD ─────────────────────── */
#define STR_DOC_TITLE   L("DOCTOR MANAGEMENT",      "医生管理")
#define STR_DOC_LIST    L("List All Doctors",        "查看所有医生")
#define STR_DOC_ADD     L("Add New Doctor",          "新增医生")
#define STR_DOC_SEARCH  L("Search Doctor by ID",     "按编号查询医生")
#define STR_DOC_DELETE  L("Delete Doctor by ID",     "按编号删除医生")
#define STR_ENTER_DID   L("Enter Doctor ID: ",       "输入医生编号：")
#define STR_COL_TITLE   L("Title",   "职称")
#define STR_COL_SEEN    L("Patients","接诊数")
#define STR_NO_DOCTORS  L("No doctors found.", "暂无医生。")
#define STR_DOC_ADDED   L("Doctor added! ID: ", "医生已添加！编号：")
#define STR_ENTER_TITLE L("Title      : ",  "职称：")

/* ───────────────────────────────── Medicine CRUD ────────────────────── */
#define STR_MED_TITLE   L("MEDICINE MANAGEMENT",    "药品管理")
#define STR_MED_LIST    L("List All Medicines",      "查看所有药品")
#define STR_MED_ADD     L("Add New Medicine",        "新增药品")
#define STR_MED_SEARCH  L("Search Medicine by ID",   "按编号查询药品")
#define STR_MED_DELETE  L("Delete Medicine by ID",   "按编号删除药品")
#define STR_ENTER_MID   L("Enter Medicine ID: ",     "输入药品编号：")
#define STR_COL_GENERIC L("Generic",  "通用名")
#define STR_COL_PRICE   L("Price",    "价格")
#define STR_COL_STOCK   L("Stock",    "库存")
#define STR_NO_MEDS     L("No medicines found.", "暂无药品。")
#define STR_MED_ADDED   L("Medicine added! ID: ", "药品已添加！编号：")
#define STR_ENTER_BRAND L("Brand Name : ",  "品牌名：")
#define STR_ENTER_GEN   L("Generic    : ",  "通用名：")
#define STR_ENTER_ALIAS L("Alias      : ",  "别名：")
#define STR_ENTER_PRICE L("Price      : ",  "价格：")
#define STR_ENTER_STOCK L("Stock      : ",  "库存：")
#define STR_ENTER_WARN  L("Warning Line: ", "预警线：")

/* ───────────────────────────────── Misc ─────────────────────────────── */
#define STR_LANG_EN     L("Language set to: English",  "语言已切换为：英文")
#define STR_LANG_ZH     L("语言已切换为：中文",         "语言已切换为：中文")

#endif /* LANG_H */
