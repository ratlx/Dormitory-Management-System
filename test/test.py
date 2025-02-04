import random

# 随机生成姓名
def generate_name():
    first_names = ["赵", "钱", "孙", "李", "周", "吴", "郑", "王", "冯", "陈", "褚", "卫", "蒋", "沈", "韩", "杨",
                   "朱", "秦", "尤", "许", "何", "吕", "施", "张", "孔", "曹", "严", "华", "金", "魏", "陶", "姜",
                   "戚", "谢", "邹", "喻", "柏", "水", "窦", "章", "云", "苏", "潘", "葛", "奚", "范", "彭", "郎",
                   "鲁", "韦", "昌", "马", "苗", "凤", "花", "方", "俞", "任", "袁", "柳", "酆", "鲍", "史", "唐",
                   "费", "廉", "岑", "薛", "雷", "贺", "倪", "汤", "滕", "殷", "罗", "毕", "郝", "邬", "安", "常",
                   "乐", "于", "时", "傅", "皮", "卞", "齐", "康", "伍", "余", "元", "卜", "顾", "孟", "平", "黄",
                   "和", "穆", "萧", "尹", "姚", "邵", "湛", "汪", "祁", "毛", "禹", "狄", "米", "贝", "明", "臧",
                   "计", "伏", "成", "戴", "谈", "宋", "茅", "庞", "熊", "纪", "舒", "屈", "项", "祝", "董", "梁"]
    last_names = ["悦", "鑫", "琪", "泽", "宇", "晨", "阳", "佳", "浩", "萱", "婷", "杰", "涵", "博", "思", "睿",
                  "欣", "梓", "航", "俊", "瑶", "霖", "熙", "毅", "昕", "婉", "豪", "靖", "依", "朗", "楠", "韵",
                  "逸", "恒", "凝", "楚", "洛", "童", "润", "舒", "景", "悦", "帆", "锦", "素", "柯", "遥", "檀",
                  "祈", "殷", "允", "沐", "知", "亦", "语", "榆", "若", "菱", "启", "卓", "梦", "育", "辰", "凯",
                  "悠", "卿", "诗", "念", "海", "波", "涛", "强", "刚", "军", "勇", "亮", "明", "辉", "平", "伟",
                  "芳", "娜", "敏", "静", "丽", "娟", "霞", "慧", "玲", "华", "梅", "龙", "凤", "飞", "鹏", "云",
                  "燕", "艳", "雪", "琴", "文", "青", "宏", "祥", "瑞", "甜", "雨", "志", "哲", "超", "磊", "健",
                  "雄", "颖", "琳", "珊", "蓉", "嘉", "源", "胜", "腾", "达", "荣", "广", "庆", "立", "章", "高",
                  "林", "森", "虎", "豹", "彪", "宝", "贝", "春", "夏", "秋", "冬", "金", "银", "铜", "铁", "玉",
                  "翠", "珠", "宝","子涵", "子轩", "雨泽", "雨薇", "浩然", "欣怡", "俊辉", "嘉怡", "梓豪", "诗涵", "博宇", "雅萱",
                  "明轩", "静怡", "睿泽", "婉婷", "皓宇", "乐萱", "晨阳", "依诺", "宇轩", "可馨", "梓轩", "佳怡",
                  "锦程", "思涵", "俊杰", "悦彤", "逸飞", "若汐", "瑞霖", "靖瑶", "泽宇", "梦瑶", "俊熙", "语桐",
                  "文昊", "紫涵", "浩宇", "语嫣", "一诺", "宇泽", "心怡", "致远", "子琪", "沐阳", "若萱", "泽熙",
                  "雨桐", "明辉", "嘉豪", "紫萱", "景辉", "雨昕", "卓轩", "慧琳", "晨熙", "俊朗", "舒雅", "逸晨",
                  "艺萱", "梓晨", "启铭", "芷涵", "浩轩", "婉晴", "锦轩", "润阳", "诗雅", "智宇", "睿晨", "嘉琪",
                  "梦琪", "宇澄", "灵萱", "泽凯", "静萱", "晨宇", "耀辉", "俊豪", "若琳", "博超", "思琪", "俊楠",
                  "雅琴", "梓萱", "雨晨", "佳琪", "明哲", "婉琳", "宇阳", "浩晨", "可昕", "文轩", "靖轩", "梓航",
                  "悦萱", "泽霖", "智轩", "诗琪", "俊凯", "静雅", "宇铭", "紫菱", "润轩", "悦瑶", "明宇", "嘉熙",
                  "梓琪", "雨嫣", "睿豪", "若瑶", "浩泽", "婉萱", "晨泽", "梓睿", "锦霖", "思瑶", "智辉", "俊逸",
                  "雅萱", "泽翰", "诗萱", "浩楠", "逸轩", "佳瑶", "明泽", "雨琪", "梓怡", "俊哲", "静萱"]
    return random.choice(first_names) + random.choice(last_names)

# 随机生成学号（格式：年份+6位数字）
def generate_student_id(existing_ids):
    while True:
        year = random.randint(2018, 2024)  # 假设从2018年到2024年入学
        suffix = random.randint(100000, 999999)
        student_id = f"{year}{suffix}"
        if student_id not in existing_ids:
            existing_ids.add(student_id)
            return student_id

# 随机生成性别
def generate_gender():
    return random.choice(["男", "女"])

# 随机生成电话号码（国内格式：1开头，11位）
def generate_phone_number():
    prefixes = ["133", "153", "173", "177", "180", "181", "189", "199", "130", "131", "132"]
    suffix = random.randint(10000000, 99999999)
    return random.choice(prefixes) + str(suffix)

# 生成随机学生信息
def generate_student(existing_ids):
    return {
        "name": generate_name(),
        "student_id": generate_student_id(existing_ids),
        "gender": generate_gender(),
        "phone": generate_phone_number()
    }
# 生成多个学生信息
def generate_students(count):
    existing_ids = set()
    return [generate_student(existing_ids) for _ in range(count)]

# 将学生信息写入文件
def write_students_to_file(students, filename):
    with open(filename, 'w', encoding='utf-8') as file:
        for student in students:
            file.write(f"{student['name']} {student['student_id']} {student['gender']} {student['phone']}\n")

# 主函数
if __name__ == "__main__":
    student_count = 1000  # 生成1000个学生信息
    students = generate_students(student_count)
    write_students_to_file(students, 'users.txt')