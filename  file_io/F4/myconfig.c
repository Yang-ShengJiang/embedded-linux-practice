#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct HashNode {
    char *key;
    char *value;        // 原来是 int，改成 char*
    struct HashNode *next;
} HashNode;

typedef struct{
    HashNode **buckets;
    int size;
}HashTable;

int hash(char *key,int size){
    int sum=0;
    while(*key){
        sum+=*key;
        key++;
    }
    return sum%size;
}
// 你改造后的哈希表接口：
HashTable *ht_create(int size){
    HashTable *ht=malloc(sizeof(HashTable));
    if(!ht) return NULL;
    ht->size=size;
    ht->buckets=malloc(sizeof(HashNode)*size);
    if(!ht->buckets){
        free(ht);
        return NULL;
    }
    for(int i=0;i<size;i++){
        ht->buckets[i]=NULL;
    }
    return ht;
}

void ht_put(HashTable *ht, char *key, char *value){
    int idx=hash(key,ht->size);
    HashNode *p=ht->buckets[idx];
    while(p){
        if(strcmp(p->key,key)==0){
            p->value=strdup(value);
            return;
        }
        p=p->next;
    }
    HashNode *node=malloc(sizeof(HashNode));
    if(!node){
        free(ht);
        return;
    }
    node->key=strdup(key);
    node->value=strdup(value);
    node->next=ht->buckets[idx];
    ht->buckets[idx]=node;
}

char *ht_get(HashTable *ht, char *key){
    int idx=hash(key,ht->size);
    HashNode *p=ht->buckets[idx];
    while(p){
        if(strcmp(p->key,key)==0){
            return strdup(p->value);
        }
        p=p->next;
    }
    return NULL;
}

void ht_destroy(HashTable *ht){
    if(!ht) return;
    HashNode *q,*p;
    for(int i=0;i<ht->size;i++){
        p=ht->buckets[i];
        while(p){
            q=p;
            p=p->next;
            free(q->key);
            free(q->value);
            free(q);
        }
    }
    free(ht->buckets);
    free(ht);
}
void ht_print(HashTable *ht){

}   // 新增：打印所有配置项

// 去除字符串首尾空白字符（空格、制表符、换行）
char *trim(char *s) {
    // 1. 跳过开头的空白
    while(*s==' '||*s=='\n'||*s=='\t'||*s=='\r') s++;
    if(*s=='\0') return s;
    // 2. 从末尾往前，把空白替换成 '\0'
    char *end=s+strlen(s)-1;
    while(end>s&&(*end==' '||*end=='\n'||*end=='\t'||*end=='\r')){
        *end='\0';
        s--;
    }
    // 3. 返回新起点
    return s;
}

// 解析配置文件
HashTable *parse_config(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return NULL;
    }

    HashTable *ht = ht_create(100);
    char line[1024];

    while (fgets(line, sizeof(line), fp)) {
        // 你的代码：
        // 1. 跳过开头的空白，得到 p
        // 2. 如果 *p 是 '#' 或 '\n' 或 '\0'，continue
        // 3. 用 strchr 找 '='
        // 4. 没有 '=' 就 continue
        // 5. 把 '=' 替换成 '\0'，key = p，value = eq + 1
        // 6. ht_put(ht, trim(key), trim(value))
    }

    fclose(fp);
    return ht;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "用法: %s <config.ini>\n", argv[0]);
        return 1;
    }

    HashTable *ht = parse_config(argv[1]);
    if (ht == NULL) return 1;

    // 1. 打印所有配置项
    // 2. 查询 server_port
    // 3. 查询一个不存在的 key

    ht_destroy(ht);
    return 0;
}