#include<stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>

#define E_VALUE 2.718281828459045
#define PI_VALUE 3.141592653589793
#define EPSILON 1e-6
#define ITERATION_NUMBER 1000

typedef enum {TOKEN_NUMBER,TOKEN_VARIABLE,TOKEN_OPERATOR,TOKEN_FUNCTION} TokenType;//Hangi tip olacağını belirliyorum.

typedef struct Token {// Her bölmesinde 2 veya 1 bilgi tutan linked list oluşturuyorum.
    TokenType type; //Token tipine union içindeki birimlerden ya birini kullacağız ya da direkt kullanmayacağız.Bundan dolayı union ile bağladım.
    union {
        double number;
        char operator;
        char *function;//Fonksiyonu çağırmak için
    } value;
    struct Token *next; //Linked listte sonraki bölüme erişmek için pointer ataması yapıyorum.
} Token;
typedef struct TokenStack{
    Token *token;
    struct TokenStack *next;
} TokenStackNode;    

typedef struct {
    Token *infix; //Düzgün yazılan notasyonun Token linked listine bağlı hali,pointer kullanma sebebim ilk düğümün nerede olacağının bilgisini tutmak.
    Token *postfix;//Ters Lehçe Notasyonunun Token linked listine bağlı hali,pointer kullanma sebebim ilk düğümün nerede olacağının bilgisini tutmak.
    char *original; //Kullanıcının girdisini depolayan birim
} ParserResult;

const char *valid_functions[] = {
    "sin", "cos", "tan", "cot", "sec", "csc",
    "arcsin", "arccos", "arctan",
    "log_", "ln", "sqrt", NULL
};
char* onevariablereadinput();//one variable read input (Kullanıcıdan input alacak olan fonksiyon)
bool parse_edges(const char* input, double* lower, double* upper);//Başka bir parser daha (bu benim bu ödeve karşı gıcıklığımdan oluşturduğum şey)
ParserResult shunting_yard_parser();//ParserResult adlı struct'ı oluşturacak olan fonksiyon
void freeTokenList(Token *head);//Linked listeyi temizleyen fonksiyon
void freeParserResult(ParserResult *result);// Stuck'ların içindeki linked listleri kendi icinde freeTokenlist fonksiyonunu çağırarak siliyor.
void printTokenList(Token *head);//Bu benim doğru çalışıyor mu diye printf yaptırdığım bir linked list printleme fonksiyonu
int precedence(char op);//İşlem önceliği tanımlayan fonksiyon
bool operator_right_associative(char op);//Operatörlerden sağ tanımlı olanları ayırmak için fonksiyon
void push(TokenStackNode **stack, Token *token);//Stacka yeni eleman koyuyor
Token* pop(TokenStackNode **stack);//Stacktan eleman siliyor
Token* peek(TokenStackNode *stack);//En üstteki elemana bakıyor.
void appendPostfix(Token **postfix, Token **last, Token *token);//topostfix fonksiyonu ile beraber çalışıyor.
Token* toPostfix(Token *infix);//infix notasyonunu postfix notasyonuna çeviriyor.
double evaluatePostfix(Token *postfix, double x_value);//Postfix notasyonunda x yerine değer koyarak içinde yazılan fonksiyonun değerini buluyor.
double trapezoidal_rule(Token *postfix, double a, double b, int n);//Sayısal integral kuralları
double simpson13_rule(Token *postfix, double a, double b, int n);
double simpson38_rule(Token *postfix, double a, double b, int n);
double **creatematrix(int n);// Matris alanı yaratan fonksiyon
double *createresultmatrix(int n);//Gauss Seidel ve Cholesky yöntemi için sonuç kısmının matrisi
double **scanfmatrix(double **matrix,int n);//Matris değerlerini alan fonksiyon
void printmatrix(double **matrix,int n);//Matrisi printf'leyen fonksiyon
void freematrix(double **matrix,int n); // Matrisi pointer ile yarattığım için bellek sızıntısını engelleyen fonksiyon
double **inversematrix(double **matrix,int n);//Verilen bir matrisin tersini bulmak için.
double *Gauss_Seidel(double **matrix,double *resultmatrix,int n);//Gauss Seidel Yöntemi Çözücü
double *Cholesky(double **matrix,double *resultmatrix,int n);//Cholesky Yöntemi Çözücü

int main(){
    int method_choice;
    int solution=0;
    char answer;
    ParserResult reader;
    char *edges;
    const char *control;
    char *end;
    double lower_limit=NAN;
    double upper_limit=NAN;
    double previous_iteration=0;
    double iteration=0;
    double f_val, f_val_minus,f_val_plus,f_derivative;
    double h;
    int iteration_counter=1;
    int lobby=0;
    double x;
    double result1;
    int counter = 2;
    int i;
    int n;
    double **inversed;
    double **mtr;
    double *mtr2;
    double *mtr3;
    double *result;
    while(lobby==0){
        printf("Welcome to numerical analysis calculator\n");
        sleep(2);
        printf("For Bisection method, 1 \n");
        printf("For Regula Falsi method, 2 \n");
        printf("For Newton-Rapshon method, 3 \n");
        printf("For inverse of NxN matrix's inverse, 4\n");
        printf("For Cholesky Method, 5 \n");
        printf("For Gauss-Seidel Method, 6 \n");//lobi
        printf("For Numerical Derivative, 7 \n");
        printf("For Simpson Method, 8\n");
        printf("For Trapezoid Method, 9 \n");
        printf("For Gregory Newton Interpolation without variable transformations, 10 \n");
        printf("For basic calculator, 11\n");
        printf("enter \n");
        scanf("%d",&lobby);
        while (getchar() != '\n');
    }
    if(lobby==1){
        printf("Define your equation\n");
        reader=shunting_yard_parser();
        printf("Define edges with string. For example [2,5]\n");
        edges=onevariablereadinput();
        if(!parse_edges(edges,&lower_limit,&upper_limit)){
            printf("Not parsed.");
            freeParserResult(&reader);
        }
        free(edges);
        if(evaluatePostfix(reader.postfix,lower_limit)*evaluatePostfix(reader.postfix,upper_limit)<0){
            iteration=(double)((lower_limit+upper_limit)/2);
            printf("%d. Iteration and result=\t%lf\n",iteration_counter,iteration);
        }
        else{
            printf("Program can't know if there is a root or not between these range.");
            freeParserResult(&reader);
            return 1;
        }
        while((iteration_counter<3||fabs(iteration-previous_iteration)>EPSILON)&& solution!=1 ){
            f_val=evaluatePostfix(reader.postfix,iteration);
            if (f_val<0){
                lower_limit=iteration;
                previous_iteration=iteration;
                iteration=(double)((lower_limit+upper_limit)/2);
                iteration_counter++;
            }
            else if(f_val>0){
                upper_limit=iteration;
                previous_iteration=iteration;
                iteration=(double)((lower_limit+upper_limit)/2);
                 iteration_counter++;
            }
            else{
                solution=1;
            }
            printf("%d. Iteration and result=\t%lf\n",iteration_counter,iteration);
        }
        freeParserResult(&reader);
        return 0;
    }
    else if(lobby==2){
        printf("Define your equation\n");
        reader=shunting_yard_parser();
        printf("Define edges with string. For example [2,5]\n");
        edges=onevariablereadinput();
        if(!parse_edges(edges,&lower_limit,&upper_limit)){
            printf("Not parsed.");
            freeParserResult(&reader);
        }
        free(edges);
        if(evaluatePostfix(reader.postfix,lower_limit)*evaluatePostfix(reader.postfix,upper_limit)>0){
            printf("Program can't know if there is a root or not between these range.");
            freeParserResult(&reader);
            return 1;
        }
        f_val_plus=evaluatePostfix(reader.postfix,upper_limit);
        f_val_minus=evaluatePostfix(reader.postfix,lower_limit);
        iteration=(double)((lower_limit*f_val_plus-upper_limit*f_val_minus)/(f_val_plus-f_val_minus));
        printf("%d. Iteration and result=\t%lf\n",iteration_counter,iteration);
        while((iteration_counter<3||fabs(iteration-previous_iteration)>EPSILON)&& solution!=1 ){
            f_val=evaluatePostfix(reader.postfix,iteration);
            if (f_val<0){
                lower_limit=iteration;
                f_val_minus=evaluatePostfix(reader.postfix,lower_limit);
                previous_iteration=iteration;
                iteration=(double)((lower_limit*f_val_plus-upper_limit*f_val_minus)/(f_val_plus-f_val_minus));
                iteration_counter++;
            }
            else if(f_val>0){
                upper_limit=iteration;
                f_val_plus=evaluatePostfix(reader.postfix,upper_limit);
                previous_iteration=iteration;
                iteration=(double)((lower_limit*f_val_plus-upper_limit*f_val_minus)/(f_val_plus-f_val_minus));
                iteration_counter++;
            }
            else{
                solution=1;
            }
            printf("%d. Iteration and result=\t%lf\n",iteration_counter,iteration);
        }
        printf("%d. Iteration and result=\t%lf",iteration_counter,iteration);
        freeParserResult(&reader);
        return 0;

    }
    else if(lobby==3){
        h=0.01;
        printf("Define your equation\n");
        reader=shunting_yard_parser();
        printf("Now, For Newton Raphson method program need a starting value\n");
        sleep(2);
        printf("If you don't give the value, then it will be 0\n");
        sleep(2);
        printf("Will you enter? Y/N\t");
        scanf(" %c",&answer);
        if(answer=='y'||answer=='Y'){
            printf("Enter value=");
            scanf("%lf",&previous_iteration);
        }
        else if(answer=='n'||answer=='N'){
            previous_iteration=0;
        }
        printf("%d. Iteration and result=\t%lf\n",iteration_counter,previous_iteration);
        while(iteration_counter<ITERATION_NUMBER && counter!=-1){
            f_val=evaluatePostfix(reader.postfix,previous_iteration);
            f_val_minus=evaluatePostfix(reader.postfix,previous_iteration-h); //Sayısal türevi int main içinde yaptım çünkü neredeyse hiç kullanmıyorum.
            f_val_plus=evaluatePostfix(reader.postfix,previous_iteration+h);
            f_derivative=(double)((f_val_plus-f_val_minus)/(2*h));
            if(fabs(f_derivative)<EPSILON){ //Eğer türev sıfır olursa payda 0 olur.İstenmeyen durum bu
                printf("Derivative can't be zero");
                freeParserResult(&reader);
                return 1;
            }
            iteration=previous_iteration-(double)f_val/f_derivative;
            if(fabs(iteration-previous_iteration)<EPSILON){
                counter=-1;
            }
            else{
            previous_iteration = iteration;
            iteration_counter++;
            }
        printf("%d. Iteration and result=\t%lf\n",iteration_counter,iteration);
        }
        freeParserResult(&reader);
        return 0;
    }
    else if(lobby==4){
        printf("Enter the size of matrix=");
        scanf("%d",&n);
        mtr=creatematrix(n);
        mtr=scanfmatrix(mtr,n);
        inversed=inversematrix(mtr,n);
        printf("The inverse of your matrix:\n");
        printmatrix(inversed,n);
        freematrix(inversed,n);
        freematrix(mtr,n);
    }
    else if(lobby==5){
        printf("Enter the size of matrix=");
        scanf("%d",&n);
        mtr=creatematrix(n);
        mtr=scanfmatrix(mtr,n);
        printf("Enter the result matrix");
        mtr2=createresultmatrix(n);
        result=Cholesky(mtr,mtr2,n);
        freematrix(mtr,n);
        free(mtr2);
        free(result);
    }
    else if(lobby==6){
        printf("Enter the size of matrix=");
        scanf("%d",&n);        
        mtr=creatematrix(n);
        mtr=scanfmatrix(mtr,n);
        printf("Enter the result matrix");
        mtr2=createresultmatrix(n);
        mtr3=Gauss_Seidel(mtr,mtr2,n);
        free(mtr3);
        freematrix(mtr,n);
        free(mtr2);
    }
    else if(lobby==7){
        printf("Define your equation\n");
        reader=shunting_yard_parser();
        printf("Enter h=");
        scanf("%lf",&h);
        printf("\nEnter number=");
        scanf("%lf",&previous_iteration);
        f_val=evaluatePostfix(reader.postfix,previous_iteration);
        f_val_minus=evaluatePostfix(reader.postfix,previous_iteration+h);
        f_val_plus=evaluatePostfix(reader.postfix,previous_iteration-h);
        f_derivative=(double)((f_val_minus-f_val_plus)/(2*h));
        printf("central differences derivative=%lf\n",f_derivative);
        f_derivative=(double)((f_val-f_val_plus)/h);
        printf("forward differences derivative=%lf\n",f_derivative);
        f_derivative=(double)((f_val_minus-f_val)/h);
        printf("back differences derivative=%lf\n",f_derivative);
        freeParserResult(&reader);
    }
    else if(lobby==8){
        printf("Define your equation\n");
        reader=shunting_yard_parser();
        printf("Define edges with string. For example [2,5]\n");
        edges=onevariablereadinput();
        if(!parse_edges(edges,&lower_limit,&upper_limit)){
            printf("Not parsed.");
            free(edges);
            freeParserResult(&reader);
            return 1;
        }
            printf("Choose method:\n");  // Her şeye parser yapamam.
            printf("1 for 1/3 simpson\n");
            printf("2 for 3/8 simpson\n");
            scanf("%d",&method_choice);
            if(method_choice==1||method_choice==2){
                printf("what is n=\t");//Method seçtiriyorum.
                scanf("%d",&n);
            }
            else{
                printf("Unknown syntax");
                freeParserResult(&reader);
                return 1;
            }
            if(method_choice==1){
                if(n%2!=0){
                    printf("You can't calculate");
                    freeParserResult(&reader);
                    return 1;
                }
                result1=simpson13_rule(reader.postfix,lower_limit,upper_limit,n);
                printf("Result=%lf",result1);
            }
            else{
                if(n%3!=0){
                    printf("You can't calculate");
                    freeParserResult(&reader);
                    return 1;
                }
                result1=simpson38_rule(reader.postfix,lower_limit,upper_limit,n);
                printf("Result=%lf",result1);
            }
            freeParserResult(&reader);
    }
    else if(lobby==9){
    printf("Define your equation:\n");
    reader = shunting_yard_parser();
    printf("Define edges with string. For example [2,5]\n");
    edges = onevariablereadinput();

    // Integral sınırlarını ayrıştır ve kontrol et
    if (!parse_edges(edges, &lower_limit, &upper_limit)) {
        printf("Invalid edge format. Use [a,b].\n");
        freeParserResult(&reader);
        free(edges);
        return 1;
    }

    printf("Enter the number of pieces:");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid input for 'n'. It must be a positive integer.\n");
        freeParserResult(&reader);
        free(edges);
        return 1;
    }
    while (getchar() != '\n'); // Buffer'ı temizle

    // Integral hesapla ve sonucu göster
    result1 = trapezoidal_rule(reader.postfix, lower_limit, upper_limit, n);
    printf("Result=%.6lf\n", result1); // Düzeltilmiş format

    // Belleği temizle
    freeParserResult(&reader);
    free(edges);
    return 0;
    }
    else if(lobby==10){
        int n_points, i, j, k;
    double *x, *y, **differences, x_value, result, product;
    
    printf("Enter number of data points: ");
    scanf("%d", &n_points);
    
    if(n_points <= 0) {
        printf("Invalid number of points!\n");
        return 1;
    }
    
    // Allocate memory for data points
    x = malloc(n_points * sizeof(double));
    y = malloc(n_points * sizeof(double));
    differences = malloc(n_points * sizeof(double *));
    
    for(i = 0; i < n_points; i++) {
        differences[i] = malloc(n_points * sizeof(double));
    }
    
    // Read data points
    printf("Enter data points in format x y:\n");
    for(i = 0; i < n_points; i++) {
        printf("Point %d: ", i+1);
        scanf("%lf %lf", &x[i], &y[i]);
        differences[i][0] = y[i];
    }
    
    // Calculate divided differences
    for(j = 1; j < n_points; j++) {
        for(i = 0; i < n_points - j; i++) {
            differences[i][j] = (differences[i+1][j-1] - differences[i][j-1]) / (x[i+j] - x[i]);
        }
    }
    
    // Get evaluation point
    printf("Enter x value to interpolate: ");
    scanf("%lf", &x_value);
    
    // Calculate interpolation using Newton's formula
    result = differences[0][0];
    for(k = 1; k < n_points; k++) {
        product = 1.0;
        for(j = 0; j < k; j++) {
            product *= (x_value - x[j]);
        }
        result += product * differences[0][k];
    }
    
    printf("Interpolated value at %f is: %f\n", x_value, result);
    
    // Clean up memory
    free(x);
    free(y);
    for(i = 0; i < n_points; i++) {
        free(differences[i]);
    }
    free(differences);
    }
    else if(lobby==11){
        printf("Define your equation\n");
        reader=shunting_yard_parser();
        f_val=evaluatePostfix(reader.postfix,0);
        printf("Answer:%f",f_val);
    }
    return 0;
}



 char* onevariablereadinput(){
    int capacity= 32; //Başlangıç kapasitesi 32, başlangıçta statik yapıp ihtiyaca göre arttırdım.Kod belki daha fazla yer kaplayacak ama daha hızlı çalışacak.
    char *array= malloc(capacity*sizeof(char));
    char *newarray;
    int length=0;
    int c;
    if(array==NULL){
        return NULL;
    }
    while((c=getchar())!= '\n' && c !=EOF){
        if(length+1>=capacity){//kapasite kontrolü
            capacity*=2;//kapasiteyi 2'şer arttırmak yerine 2 katına çıkara çıkara ilerledim.Daha az realloc kullanmak için.Daha hızlı olsun diye.
            newarray=realloc(array,capacity);//Yeni pointer kullanımı ile bellek sızıntısını ve boş bellek kalmasını önlüyorum.
            if(newarray==NULL){
                free(array);//Serbest bıraktım.
                return NULL;
            }
            array=newarray;
        }
        array[length]=c;//Dizinin o kısmına yazmam gereken charı öğreniyorum.
        length++;
    }
    array[length]='\0';
    return array;    //Dışarıya string çıktısı vermek için return dizi yaptım.
 }

 bool parse_edges(const char* input, double* lower, double* upper) {
    const char* control = input; //Sınırları bile string ile alıyorum. Delirdim resmen.
    char* end;

    // Başlangıçta NaN atayalım 
    *lower = *upper = NAN;

    // Boşlukları atlıyoruz
    while (isspace(*control)) control++;

    // [ kontrolü
    if (*control != '[') return false;
    control++;

    // Lower limit değerini öğrenme
    *lower = strtod(control, &end);
    if (control == end) return false; // Sayı okunamadı
    control = end;

    // Virgül kontrolü
    while (isspace(*control)) control++;
    if (*control != ',') return false;
    control++;

    // Upper limit öğrenme
    *upper = strtod(control, &end);
    if (control == end) return false;
    control = end;

    // ] kontrolü ve sonrasında sadece boşluk olmalı
    while (isspace(*control)) control++;
    if (*control != ']') return false;
    control++;
    while (isspace(*control)) control++;
    
    return (*control == '\0'); // String sonuna varıyoruz.
}

 ParserResult shunting_yard_parser(){
    ParserResult result = {NULL, NULL, NULL};//Başlangıç için değişken oluşturup hepsini NULL yaptım.
    char *input = onevariablereadinput();//inputu alıyorum fonksiyonu çağırarak.
    if (input==NULL){//Bilgi gelmiş mi diye kontrol (ayrıca boş input için de yararlı)
        return result;
    }
    result.original = input; //Struct'daki result degiskeninin original kısmına inputu atıyorum.
    //Önce girişin doğru olup olmadığını kontrol ettim. Sonra Token infix linked listini oluşturdum.
    Token *infix = NULL; //infix adında linked list başını oluşturdum.(okunması kolay olsun diye infix dedim.)
    Token **infix_current = &infix;//Linked listte ekleme yapmayı kolaylaştırmak için
    Token *postfix=NULL;
    Token **postfix_current=&postfix;
    const char *control = input; // inputun içinde gezineceğimiz bir char pointeri
    double num;//Sayısal sonuçlar için
    const char *start;
    char *function;
    int length;
    bool valid=false;
    bool error=false;
    int operator_precedence;
    int i=0;
    while(*control!='\0'){
        if(isspace(*control)){
            control++;
        }
        if(isdigit(*control)||(*control=='e')||((*control=='p')&&(*(control+1)=='i'))){ //stringte şu anda bulunduğum char'ın sayı tipinde olup olmadığını öğreniyorum.
            if(*control=='e'){ 
                num=E_VALUE;
                control++;
            }
            else if(*control=='p' && *(control+1)=='i'){
                num=PI_VALUE;
                control+=2;
            }
            else{
                char *end;
                num=strtod(control,&end);
                control=end;
            }
            Token *tmp=malloc(sizeof(Token));//bulduktan sonra yeni bir node yaratıyorum ve onu infix noduna bağlıyorum bilgileri ile birlikte.
            tmp->type=TOKEN_NUMBER;
            tmp->value.number=num;
            tmp->next=NULL;
            *infix_current=tmp;
            infix_current=&tmp->next;   
        }
        else if (*control == 'x') {//Değişkeni buldum.Yeni bir node yaratıyorum ve onu infix noduna bağlıyorum bilgileri ile birlikte.
            Token *tmp = malloc(sizeof(Token));
            tmp->type = TOKEN_VARIABLE;
            tmp->next = NULL;
            *infix_current = tmp;
            infix_current = &tmp->next;
            control++;
        }
        else if(strchr("+-*/^()",*control)){ //Operatorleri buluyorum. Yeni bir node yaratıyorum ve onu infix noduna bağlıyorum bilgileri ile birlikte.
            Token *tmp = malloc(sizeof(Token));
            tmp->type = TOKEN_OPERATOR;
            tmp->value.operator = *control;
            tmp->next = NULL;
            *infix_current = tmp;
            infix_current = &tmp->next;
            control++;
        }
        else if(isalpha(*control)){
            start=control;
            while(isalpha(*control)||*control=='_'){
                control++;
            }//Fonksiyon kontrolü yapıyorum.
            length=control-start;
            function=malloc(length+1);
            if(function==NULL){
                return result;
            }
            strncpy(function, start, length);
            function[length] = '\0';
            while (valid_functions[i] != NULL && !valid) {
                if (strcmp(function, valid_functions[i]) == 0) {
                    valid = true;
                }
                i++;
            }
            if (!valid) {
                printf("Error,Wrong functiion name'%s'\n", function);
                free(function);
                result.original = NULL;
                return result;
            }
            Token *tmp=malloc(sizeof(Token));
            tmp->type = TOKEN_FUNCTION;
            tmp->value.function =function;
            tmp->next = NULL;
            *infix_current = tmp;
            infix_current = &tmp->next;
        }
        else{
            printf("Error,unknown Syntax");
            error=true;
        }
        if(error){
            freeTokenList(infix);
            return(ParserResult){0};
        }
    }
    result.infix=infix;//İnfix notasyonu bitirdim.
    //Postfix notasyona geçeceğim.
    result.postfix=toPostfix(infix);
    return result;
}

int precedence(char op) {
    switch(op) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        case '^': return 3;
        default: return 0;
    }
}

bool operator_right_associative(char op) {
    return (op == '^');
}

void push(TokenStackNode **stack, Token *token) {
    TokenStackNode *new_node = malloc(sizeof(TokenStackNode));
    new_node->token = token;
    new_node->next = *stack;
    *stack = new_node;
}

Token* pop(TokenStackNode **stack) {
    if(!stack || !*stack){
        return NULL;
    } 
    TokenStackNode *top = *stack;
    Token *popped = top->token;
    *stack = top->next;
    free(top);
    return popped;
}

Token* peek(TokenStackNode *stack) {
    if(stack == NULL){
        return NULL;
    }
    return stack->token;
}

void appendPostfix(Token **postfix, Token **last, Token *token) {
    Token *newToken = malloc(sizeof(Token));
    *newToken = *token; // copy token content
    newToken->next = NULL;
    if(token->type == TOKEN_FUNCTION) {
        newToken->value.function = strdup(token->value.function);
    }
    if(*postfix == NULL) {
        *postfix = newToken;
        *last = newToken;
    } else {
        (*last)->next = newToken;
        *last = newToken;
    }
}

Token* toPostfix(Token *infix) {
    TokenStackNode *stack = NULL;
    Token *postfix = NULL, *last = NULL;
    Token *current = infix;

    while(current) {
        switch(current->type) {
            case TOKEN_NUMBER:
            case TOKEN_VARIABLE:
                appendPostfix(&postfix, &last, current);
                break;

            case TOKEN_FUNCTION:
                push(&stack, current);
                break;

            case TOKEN_OPERATOR: {
                char op = current->value.operator;
                
                if(op == '(') {
                    push(&stack, current);
                } 
                else if(op == ')') {
                    while(peek(stack) && peek(stack)->value.operator != '(') {
                        appendPostfix(&postfix, &last, pop(&stack));
                    }
                    if(!peek(stack)) {
                        printf("Hatali parantez!\n");
                        freeTokenList(postfix);
                        return NULL;
                    }
                    pop(&stack); // '(' i çıkar
                    
                    // Fonksiyon kontrolü
                    if(peek(stack) && peek(stack)->type == TOKEN_FUNCTION) {
                        appendPostfix(&postfix, &last, pop(&stack));
                    }
                }
                else {
                    while(peek(stack) && (precedence(op) < precedence(peek(stack)->value.operator) ||(precedence(op) == precedence(peek(stack)->value.operator) &&!operator_right_associative(op)))){
                        appendPostfix(&postfix, &last, pop(&stack));
                    }
                    push(&stack, current);
                }
                break;
            }
        }
        current = current->next;
    }

    // Kalan operatörleri boşalt
    while(peek(stack)) {
        if(peek(stack)->value.operator == '(') {
            printf("Hatali parantez!\n");
            return NULL;
        }
        appendPostfix(&postfix, &last, pop(&stack));
    }

    return postfix;
}

void freeTokenList(Token *head) {
    while (head != NULL) {
        Token *next = head->next;
        
        // Özel durum: TOKEN_FUNCTION için ekstra free
        if (head->type == TOKEN_FUNCTION && head->value.function != NULL) {
            free(head->value.function);
        }
        
        free(head);
        head = next;
    }
}

double trapezoidal_rule(Token *postfix, double a, double b, int n){
    double h = (b - a) / n;
    double sum = evaluatePostfix(postfix, a) + evaluatePostfix(postfix, b);//değerlerini postfix notasyonunda hesaplıyorum.
    
    for(int i = 1; i < n; i++) {
        double x = a + i * h;
        sum += 2 * evaluatePostfix(postfix, x);
    }
    
    return (h / 2) * sum;
}

double simpson13_rule(Token *postfix, double a, double b, int n) {
    double h = (b - a) / n;
    double sum = evaluatePostfix(postfix, a) + evaluatePostfix(postfix, b);
    
    for(int i = 1; i < n; i++) {
        double x = a + i * h;
        sum += (i % 2 == 0) ? 2 * evaluatePostfix(postfix, x) : 4 * evaluatePostfix(postfix, x);//değerlerini postfix notasyonunda hesaplıyorum.
    }
    
    return (h / 3) * sum;
}

double simpson38_rule(Token *postfix, double a, double b, int n) {
    double h = (b - a) / n;
    double sum = evaluatePostfix(postfix, a) + evaluatePostfix(postfix, b);
    
    for(int i = 1; i < n; i++) {
        double x = a + i * h;
        if(i % 3 == 0) {
            sum += 2 * evaluatePostfix(postfix, x);
        } else {
            sum += 3 * evaluatePostfix(postfix, x);
        }
    }
    
    return (3 * h / 8) * sum;
}

void freeParserResult(ParserResult *result) {
    if (result == NULL) return;

    // Önce listeleri temizliyor
    freeTokenList(result->infix);
    freeTokenList(result->postfix);
    
    // Sonra string'i temizliyor.
    free(result->original);

    // En son struct'ı sıfırlıyor.
    *result = (ParserResult){NULL, NULL, NULL};
}
void printTokenList(Token *head) {
    Token *current = head;
    printf("Postfix Linked List: [");
    
    while (current != NULL) {
        switch (current->type) {
            case TOKEN_NUMBER:
                printf("%f", current->value.number); // Sayısal değer
                break;
            case TOKEN_VARIABLE:
                printf("x"); // Değişken
                break;
            case TOKEN_OPERATOR:
                printf("%c", current->value.operator); // Operatör (+, -, *, /, ^)
                break;
            case TOKEN_FUNCTION:
                printf("%s", current->value.function); // Fonksiyon (sin, cos, log)
                break;
        }
        if (current->next != NULL) {
            printf(" -> "); // Düğümler arası ok işareti
        }   
        current = current->next;
    }
    
    printf("]\n"); // Listenin sonu
}

double evaluatePostfix(Token *postfix, double x_value){
    int length=2;
    // Dinamik stack yapısı
    typedef struct {
        double *data;
        int capacity;
        int top;
    } Stack;

    // Stack oluşturma
    Stack s;
    s.capacity = 10; // Başlangıç kapasitesi
    s.data = malloc(s.capacity * sizeof(double));
    s.top = -1;

    // Stack işlemleri için yardımcı fonksiyonlar

    void push(Stack *s, double val) {
        if (s->top == s->capacity - 1) {
            s->capacity *= length;
            s->data = realloc(s->data, s->capacity * sizeof(double));
            if (!s->data) {
                fprintf(stderr, "Memory allocation failed!\n");
                exit(EXIT_FAILURE);
            }
        }
        s->data[++s->top] = val;
    }

    double pop(Stack *s) {
        if (s->top < 0) {
            fprintf(stderr, "Stack underflow!\n");
            exit(EXIT_FAILURE);
        }
        return s->data[s->top--];
    }

    Token *current = postfix;
    while (current != NULL) {
        switch (current->type) {//Token tipinde işlem yapabilmek için token tipini case ile seçiyorum.
            case TOKEN_NUMBER:
                push(&s, current->value.number);
                break;

            case TOKEN_VARIABLE:
                push(&s, x_value);
                break;

            case TOKEN_OPERATOR: {
                double b = pop(&s);
                double a = pop(&s);
                switch (current->value.operator) {
                    case '+': push(&s, a + b); break;
                    case '-': push(&s, a - b); break;//Queue'den 2 eleman alıp operatöre göre çıkacak sonucu stack'a push ediyorum.
                    case '*': push(&s, a * b); break;
                    case '/': 
                        if (fabs(b) < EPSILON) {
                            fprintf(stderr, "Division by zero!\n");
                            exit(EXIT_FAILURE);
                        }
                        push(&s, a / b); 
                        break;
                    case '^': push(&s, pow(a, b)); break;
                    default:
                        fprintf(stderr, "Unknown operator: %c\n", current->value.operator);
                        exit(EXIT_FAILURE);//Hatalı davranmış fonksiyon beklenmedik bir şekilde.Programı kapatıyor.
                }
                break;
            }

            case TOKEN_FUNCTION: { //Token listesindeki fonksiyon tokenlerin değerlerini öğreniyorum.
                double val = pop(&s);
                if (strcmp(current->value.function, "sin") == 0) {
                    push(&s, sin(val));
                }
                else if (strcmp(current->value.function, "cos") == 0) {
                    push(&s, cos(val));
                }
                else if (strcmp(current->value.function, "tan") == 0) {
                    push(&s, tan(val));
                }
                else if (strcmp(current->value.function, "cot") == 0) {
                    push(&s, 1.0/tan(val));
                }
                else if (strcmp(current->value.function, "sec") == 0) {
                    push(&s, 1.0/cos(val));
                }
                else if (strcmp(current->value.function, "csc") == 0) {
                    push(&s, 1.0/sin(val));
                }
                else if (strcmp(current->value.function, "arcsin") == 0) {
                    push(&s, asin(val));
                }
                else if (strcmp(current->value.function, "arccos") == 0) {
                    push(&s, acos(val));
                }
                else if (strcmp(current->value.function, "arctan") == 0) {
                    push(&s, atan(val));
                }
                else if (strcmp(current->value.function, "log_") == 0) {
                    double base = pop(&s); //Logaritmanın base ile beraber olduğunu, bu sayede 2 bilgi sakladığını biliyorum.
                    push(&s, log(val)/log(base));
                }
                else if (strcmp(current->value.function, "ln") == 0) {
                    push(&s, log(val));
                }
                else if (strcmp(current->value.function, "sqrt") == 0) {
                    push(&s, sqrt(val));
                }
                else {
                    fprintf(stderr, "Unknown function: %s\n", current->value.function);
                    exit(EXIT_FAILURE);
                }
                break;
            }

            default:
                fprintf(stderr, "Unknown token type\n");
                exit(EXIT_FAILURE);
        }
        current = current->next;
    }

    if (s.top != 0) {
        fprintf(stderr, "Invalid expression (stack has %d elements)\n", s.top+1);
        exit(EXIT_FAILURE);
    }

    double result = pop(&s);
    free(s.data);
    return result;
}


double *createresultmatrix(int n){
    int i;
    double *resultmatrix=malloc(n*sizeof(double)); //Boş 1xN'lik matris yaratıyorum.Ve Memory leak oluşmasın diye hep kontrol ediyorum.
    if(resultmatrix==NULL){
        printf("Memory allocation error!");
        free(resultmatrix);
        return NULL;
    }
    printf("Enter results one by one\n");
    for(i=0;i<n;i++){                       //Girdi alıyorum.
        printf("resultmatrix[%d]=",i+1);
        scanf("%lf",&resultmatrix[i]);
    }
    return resultmatrix;
}

double **creatematrix(int n){
    int i;
    double **matrix=malloc((n*sizeof(double*)));//NxN'lik boş bir matris yaratıyorum ve memory leak oluşmasın diye hep kontrol ediyorum.
    if(matrix==NULL){
        printf("Memory allocation error!");
        return NULL;
    }
    for(i=0;i<n;i++){
        matrix[i]=malloc(n*sizeof(double));
        if(matrix[i]==NULL){
            printf("Memory allocation error!");
            freematrix(matrix,n);
            return NULL;
        }
    }
    return matrix;
}

double **scanfmatrix(double **matrix,int n){
    int i,j;
    printf("Enter matrix values\n");
    sleep(1);
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){                   //Matrise eleman girdisi alıyorum.
            printf("matrix[%d][%d]=",i+1,j+1);
            scanf("%lf",&matrix[i][j]);
        }
    }
    return matrix;
}

void printmatrix(double **matrix,int n){
    int i,j;
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){ //Matris printliyorum.
            printf("%lf\t",matrix[i][j]);
        }
        printf("\n");
    }
}

void freematrix(double **matrix,int n){
    int i;
    if (matrix == NULL){
        return;
    } 
    for(i=0;i<n;i++){
    free(matrix[i]);//İşlemlerim bittiği zaman oluşacak olan memory leak'i engelliyorum.
    }
    free(matrix);
}

double **inversematrix(double **matrix,int n){ //Matrisin tersini alacağım.
    int i,j,k;
    int control;
    double *temp;
    double pivot,ratio;
    double **inversed=creatematrix(n);    
    double **bigmatrix=malloc(n*sizeof(double*));
    if(bigmatrix==NULL){
        printf("Memory allocation error!");
        return NULL;
    }
    for (i = 0; i < n; i++) {
    bigmatrix[i] = NULL;  // NULL ile başlat
    }
    for(i=0;i<n;i++){
        bigmatrix[i]=malloc(2*n*sizeof(double));
        if(bigmatrix[i]==NULL){
            printf("Memory allocation error!");
            freematrix(bigmatrix,n);
            return NULL;
        }
    }
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            bigmatrix[i][j]=matrix[i][j];
            bigmatrix[i][j+n]=(i==j) ? 1:0;
        }
    }
    control=0;
    for(i=0;i<n;i++){
        if(fabs(bigmatrix[i][i])>0){
            control=i;
        }
        if(bigmatrix[i][i]==0){
            control=-1;
            j=i+1;
            while(j<n && control==-1){
                if(fabs(bigmatrix[j][i])>EPSILON){
                    control=j;
                }
                j++;
            }
            if(control==-1){
                printf("There is no number except zero in column %d, there is no inverse matrix.",i+1);
                freematrix(bigmatrix,n);
                return NULL;
            }
            else if(control!=i){
                temp=bigmatrix[i];
                bigmatrix[i]=bigmatrix[control];
                bigmatrix[control]=temp;
            }
        }
        pivot=bigmatrix[i][i];
        for(j=0;j<2*n;j++){
           bigmatrix[i][j]/=pivot;
        }
        for(k=0;k<n;k++){
            if(k!=i){
                ratio=bigmatrix[k][i];
                for(j=0;j<2*n;j++){
                    bigmatrix[k][j]-=ratio*bigmatrix[i][j];
                }
            }
        }       
    }
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            inversed[i][j]=bigmatrix[i][j+n];
        }
    }
    freematrix(bigmatrix,n);
    return inversed;
}

double *Gauss_Seidel(double **matrix , double *resultmatrix , int n){
    int used_before = 0;
    int counter=1;
    int counter2=1;
    char c;//cevap almak için
    int i,j,k;
    double value;
    int storage;
    double max;
    double *variablematrix=malloc(n*sizeof(double)); //n. iterasyondaki değişkenlerin değerleri
    if(variablematrix==NULL){
        printf("Memory allocation error!");
        return NULL;         
    }
    double *previousvariablematrix=malloc(n*sizeof(double));// n-1. iterasyondaki değişkenlerin değerleri
    if(previousvariablematrix==NULL){
        printf("Memory allocation error!");
        return NULL;         
    }
    double **copymatrix = creatematrix(n);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            copymatrix[i][j] = NAN;
        }
    }
    for (i = 0; i < n; i++) {
        max = 0;
        storage = 0;
        for (j = 0; j < n; j++) {
            value = fabs(matrix[i][j]);
            if (value > max) {
                max = value;
                storage = j;
            }
        }

    // Kullanılmış mı kontrolü
        j=0;
        while(j<i&&used_before==0){//Matrix double ile tanımlı olmuş olduğundan kullanıcı dışarıdan eleman girerek yanlışlıkla hata yaptırmasın diye elemanlarını NAN yaptım.
            if (!isnan(copymatrix[0][j]) && copymatrix[0][j] == matrix[0][storage]) {
            used_before = 1;   
            }
            j++;         
        }

    if (used_before) {
        printf("You can't do Gauss-Seidel\n");
        freematrix(copymatrix, n);
        freematrix(matrix, n);
        free(variablematrix);
        free(previousvariablematrix);
        return NULL;
    }

    // Artık güvenle kopyalayabiliriz
    for (k = 0; k < n; k++) {
        copymatrix[k][i] = matrix[k][storage];
    }
}


    printf("Will you give first iteration's every variable's value or not?\n");
    printf("If you don't give value ,then first iteration's every variable's value will be 0\n");
    printf("Will you give? Y/N\n");
    scanf(" %c",&c);
    if(c=='Y'||c=='y'){
        variablematrix=createresultmatrix(n);
    }
    else if(c=='N'||c=='n'){
        for(i=0;i<n;i++){
            variablematrix[i]=0;
        }
    }
    else{
        printf("Undefined input");
        freematrix(copymatrix,n);
        freematrix(matrix,n);
        free(variablematrix);
        free(previousvariablematrix);
        return NULL;
    }
    storage=1;// Fonksiyonun bu kısmında iterasyon adımını tutacağım bu değişken ile.
    for(i=0;i<n;i++){
        previousvariablematrix[i]=variablematrix[i];
    }
    while(counter==1){
        counter2=1;
        for(i=0;i<n;i++){
            value=resultmatrix[i];
            for(j=0;j<n;j++){
                if(j!=i){
                    value=value-variablematrix[j]*copymatrix[i][j];
                }
            }
            variablematrix[i]=(double)value/copymatrix[i][i];
        }
        storage++;
        k=0;
        while(counter==1 && fabs(variablematrix[k]-previousvariablematrix[k])<EPSILON){
            if(k<n){
                k++;
            }
            else{
                counter=0;
            }
        }
        if(fabs(variablematrix[k]-previousvariablematrix[k])>=EPSILON){
            for(i=0;i<n;i++){
                printf("%d. Iteration and result=\t%lf\n",storage,variablematrix[i]);
                previousvariablematrix[i]=variablematrix[i];
            }
        }
        else{
            counter=0;
        }
    }
    printf("Ended in %d. iteration\n",storage);
    for(i=0;i<n;i++){
        printf("%lf\n",variablematrix[i]);
    }
    freematrix(copymatrix,n);
    free(previousvariablematrix);
    return variablematrix;
}

double *Cholesky(double **matrix, double *resultmatrix, int n) {
    double **Lmatrix = creatematrix(n);//Üst ve alt matrisi yaratıyorum.
    double **Umatrix = creatematrix(n);
    double *ymatrix = (double *)malloc(n * sizeof(double)); //2'li ayrım için gerekecek olan iki yeni 1xN'lik matris
    double *xmatrix = (double *)malloc(n * sizeof(double));
    int i, j, k;
    double sum = 0;

    // Umatrix köşegenini 1 yap
    for (i = 0; i < n; i++) {
        Umatrix[i][i] = 1.0;
    }

    for (j = 0; j < n; j++) {
        for (i = j; i < n; i++) {
            sum = 0;
            for (k = 0; k < j; k++) {
                sum += Lmatrix[i][k] * Umatrix[k][j];
            }
            Lmatrix[i][j] = matrix[i][j] - sum;
        }

        for (i = j + 1; i < n; i++) {  // i=j+1'den başlamalı, köşegen zaten 1
            sum = 0;
            for (k = 0; k < j; k++) {
                sum += Lmatrix[j][k] * Umatrix[k][i];
            }
            if (Lmatrix[j][j] == 0) {
                printf("Zero pivot encountered at L[%d][%d]\n", j, j);
                exit(EXIT_FAILURE);
            }
            Umatrix[j][i] = (matrix[j][i] - sum) / Lmatrix[j][j];
        }
    }

    // İleri yer değiştirme: L * y = b
    for (i = 0; i < n; i++) {
        sum = 0;
        for (j = 0; j < i; j++) {
            sum += Lmatrix[i][j] * ymatrix[j];
        }
        ymatrix[i] = (resultmatrix[i] - sum) / Lmatrix[i][i];
    }

    // Geri yer değiştirme: U * x = y
    for (i = n - 1; i >= 0; i--) {
        sum = 0;
        for (j = i + 1; j < n; j++) {
            sum += Umatrix[i][j] * xmatrix[j];
        }
        xmatrix[i] = ymatrix[i] - sum;  // Umatrix[i][i] = 1 olduğu için bölme yok
    }

    // Bellekteki matrisleri serbest bırakıyorum.
    for (i = 0; i < n; i++) {
        printf("%lf\n",xmatrix[i]);
    }
    freematrix(Lmatrix,n);
    freematrix(Umatrix,n);
    free(ymatrix);
    return xmatrix;
}


