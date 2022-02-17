#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRING 50

typedef struct KeyProduct {
    char name[MAX_STRING];
    int id;
} KeyProduct;

typedef struct InfsProduct {
    float purchase_value;
    float sale_value;
    int stock;
    int sold_amount;
    int deleted;
} InfsProduct;

typedef struct Product {
    KeyProduct key;
    InfsProduct infs;
} Product;

// Menu strings
void show_introdution();
void show_menu();

// First access
void register_products(FILE *f_key, FILE *f_infs);

// Menu methods
void sell_product(FILE *f_key, FILE *f_infs);
void add_product(FILE *f_key, FILE *f_infs);
void remove_product();
void check_stock(FILE *f_key, FILE *f_infs);
void consult_profit(FILE *f_key, FILE *f_infs);
void replenish_stock();
void exit_app();
// Quicksort
void quicksort(KeyProduct *v, int n);
void sort(int Esq, int Dir, KeyProduct *v);
void partition(int Esq, int Dir, int *i, int *j, KeyProduct *v);

// utils
int is_empty_file(FILE *f_ptr);
void sort_keys(FILE *f_key);

int main() {

    int option;

    show_introdution();

    FILE *f_key = fopen("chave.bin", "ab+");
    FILE *f_infs = fopen("infs.bin", "ab+");

    if (is_empty_file(f_key))
        register_products(f_key, f_infs);

    do {
        show_menu();

        scanf("%d", &option);

        switch (option) {
            case 1:
                sell_product(f_key, f_infs);
                break;
            
            case 2:
                add_product(f_key, f_infs);
                break;

            case 3:
                
                break;

            case 4:
                check_stock(f_key, f_infs);
                break;

            case 5:
                consult_profit(f_key, f_infs);
                break;

            case 6:
                
                break;

            case 0:
                exit_app();
                break;
            
            default:
                break;
        }

    } while (option != 0);
    
    fclose(f_key);
    fclose(f_infs);

    return 0;
}

void show_introdution() {
    printf("-------------------------------------------\n");
    printf("Bem vindo a cantina! \n");
}

void show_menu() {
    printf("-------------\n");
    printf("\nMenu de Opcoes \n");
    printf("\nO que deseja fazer? \n");
    printf("Digite: \n\n");
    printf("(1) Vender\n");
    printf("(2) Cadastrar novo produto\n");
    printf("(3) Remover produto do estoque\n");
    printf("(4) Consultar estoque\n");
    printf("(5) Consultar lucro\n");
    printf("(6) Repor estoque\n");
    printf("(0) Sair\n");
    printf("\nOpcao: ");
}

void sell_product(FILE *f_key, FILE *f_infs) {

    KeyProduct *keys;
    InfsProduct inf;
    char name[MAX_STRING];
    int tam, i, exist = 0, pos, option, qnt;

    printf("-------------\n");

    // open file to read
    f_key = fopen("chave.bin", "rb");

    // calculate size of key file
    fseek(f_key, 0, SEEK_END);
    tam = (int)ftell(f_key)/sizeof(KeyProduct);

    // alloc with keys length
    keys = malloc(tam * sizeof(KeyProduct));
    
    // set pointer to start on key file
    fseek(f_key, 0, SEEK_SET);

    // mapping
    for (i = 0; i < tam; i++)
        fread(&keys[i], sizeof(KeyProduct), 1, f_key);

    // close read mode
    fclose(f_key);

    printf("\nDigite o nome do produto que gostaria de comprar: ");
    scanf("%s", name);


    for (i = 0; i < tam; i++) {
        if (strcmp(name, keys[i].name) == 1) {
            exist = 1;
            pos = i;
            break;
        }
    }

    if (exist) {
        f_infs = fopen("chave.bin", "ab");

        fseek(f_infs, 0, pos);
        fread(&inf, sizeof(InfsProduct), 1, f_infs);

        
        printf("Nome: %s\n", name);
        printf("Preço: R$ %.2f\n", inf.sale_value);

        printf("\nQuantidade desejada: ");
        scanf("%d", &qnt);

        if (qnt <= inf.stock) {
            printf("Preço total: R$ %.2f", inf.sale_value * qnt);
        }

        printf("\nDesenha comprar o produto? Digite (1) sim ou (0) nao: ");
        scanf("%d", &option);
        
        if (option) {
            inf.sold_amount += 1;
            inf.stock -= 1;
            fseek(f_infs, 0, pos);
            fwrite(&inf, sizeof(InfsProduct), 1, f_infs);
            printf("Compra realizada com sucesso!");
        } 

        fclose(f_infs);

    } else 
        printf("Produto não encontrado.\n");

}

void add_product(FILE *f_key, FILE *f_infs) {

    Product product;

    // open files
    f_key = fopen("chave.bin", "ab");
    f_infs = fopen("infs.bin", "ab");

    printf("-------------\n");

    // get informations
    printf("Preencha para adicionar o produto: \n");

    printf("\nNome do produto: ");
    scanf("%s", product.key.name);

    printf("Valor de compra: ");
    scanf("%f", &product.infs.purchase_value);

    printf("Valor de venda: ");
    scanf("%f", &product.infs.sale_value);

    printf("Quantidade em estoque: ");
    scanf("%d", &product.infs.stock);
    
    printf("Quantidade vendida: ");    
    scanf("%d", &product.infs.sold_amount);

    product.infs.deleted = 0;
    product.key.id = ftell(f_infs);

    // save
    fwrite(&product.infs, sizeof(InfsProduct), 1, f_infs);
    fwrite(&product.key, sizeof(KeyProduct), 1, f_key);

    //close files
    fclose(f_key);
    fclose(f_infs);

    sort_keys(f_key);

}

void remove_product() {

}

void check_stock(FILE *f_key, FILE *f_infs) {

    KeyProduct key;
    InfsProduct infs;

    // open files
    f_key = fopen("chave.bin", "rb");
    f_infs = fopen("infs.bin", "rb");
        
    while (fread(&key, sizeof(KeyProduct), 1, f_key) == 1) {

        printf("-------------\n");

        printf("Nome: %s\n", key.name);

        fseek(f_infs, 0, key.id);
        fread(&infs, sizeof(InfsProduct), 1, f_infs);

        printf("Valor total: R$ %.2f\n", infs.sale_value);
        printf("Estoque: %d\n", infs.stock - infs.sold_amount);
    }

    printf("\n");

    // close files
    fclose(f_key);
    fclose(f_infs);

}

void consult_profit(FILE *f_key, FILE *f_infs) {

    KeyProduct key;
    InfsProduct infs;
    float profit;

    // open files
    f_key = fopen("chave.bin", "rb");
    f_infs = fopen("infs.bin", "rb");
        
    while (fread(&key, sizeof(KeyProduct), 1, f_key) == 1) {

        printf("-------------\n");

        printf("Nome: %s\n", key.name);

        fseek(f_infs, 0, key.id);
        fread(&infs, sizeof(InfsProduct), 1, f_infs);

        profit = (infs.sold_amount * infs.sale_value) - (infs.sold_amount * infs.purchase_value);

        printf("Valor de venda: R$ %.2f\n", infs.sale_value);
        printf("Valor de compra: R$ %.2f\n", infs.purchase_value);
        printf("Quantidade vendida: %d\n", infs.sold_amount);
        printf("Lucro: R$ %.2f\n", profit);
    }

    printf("\n");

    // close file 
    fclose(f_key);
    fclose(f_infs);

}

void replenish_stock() {

}

void exit_app() {
    printf("Finalizando...\n");
}

// first access
void register_products(FILE *f_key, FILE *f_infs) {
    
    int n, i;

    printf("Quantos produtos deseja incluir: ");
    scanf("%d", &n);

    for (i = 0; i < n; i++)
        add_product(f_key, f_infs);

    printf("Produtos adicionados com sucesso! \n");
}
// Sort

void partition(int Esq, int Dir, int *i, int *j, KeyProduct *v) {
    
    KeyProduct x, w;
    
    *i = Esq; *j = Dir;

    //pivô
    x = v[(*i + *j)/2];

    do {      
        while(strcmp(x.name, v[*i].name) > 0)
            (*i)++;
    
        while(strcmp(x.name, v[*j].name) < 0)
            (*j)--;

        if (*i <= *j) {

            w = v[*i];
            v[*i] = v[*j];
            v[*j] = w;
            (*i)++;
            (*j)--;
        }

    } while (*i <= *j);
}
void sort(int Esq, int Dir, KeyProduct *v) {

    int i,j;

    partition(Esq, Dir, &i, &j, v);
    
    if (Esq < j)
        sort(Esq, j, v);
    if (i < Dir)
        sort(i, Dir, v);
}

void quicksort(KeyProduct *v, int n) {
    sort(0, n-1, v);
}


// Utils

int is_empty_file(FILE *f_ptr) {
    int size = ftell(f_ptr);

    if (size == 0)
        return 1;

    return 0;
}

void sort_keys(FILE *f_key) {

    KeyProduct *keys;
    int i;
    int tam;

    // open file to read
    f_key = fopen("chave.bin", "rb");

    // calculate size of key file
    fseek(f_key, 0, SEEK_END);
    tam = (int)ftell(f_key)/sizeof(KeyProduct);

    // alloc with keys length
    keys = malloc(tam * sizeof(KeyProduct));
    
    // set pointer to start on key file
    fseek(f_key, 0, SEEK_SET);

    // mapping
    for (i = 0; i < tam; i++)
        fread(&keys[i], sizeof(KeyProduct), 1, f_key);

    // close read mode
    fclose(f_key);

    // sort
    quicksort(keys, tam);

    // open file to update with sorted keys
    f_key = fopen("chave.bin", "wb");

    for (i = 0; i < tam; i++)
        fwrite(&keys[i], sizeof(KeyProduct), 1, f_key);

    fclose(f_key);
    

    free(keys);
}