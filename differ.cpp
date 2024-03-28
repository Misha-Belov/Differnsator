#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct node
{
    int type;
    double value;

    node * no;
    node * yes;
};

void save_tree(node * root, FILE* file);
void tree_destr(node * root);
void scan_tree(node * root, FILE* file);
void fill_tree(node * root, char * string, size_t * pos);
void draw_tree(node * root, FILE* file);
void draw_node(node * root, FILE* file);

int main()
{
    node * root = (node *) calloc(1, sizeof(node));

    FILE* tree_scan = fopen("tree_save.txt", "rb");
    scan_tree(root, tree_scan);
    fclose(tree_scan);

    FILE* tree_draw = fopen("tree_draw.dot", "wb");
    draw_tree(root, tree_draw);
    fclose(tree_draw);

    FILE* tree_save = fopen("tree_save2.txt", "wb");
    save_tree(root, tree_save);
    fclose(tree_save);

    tree_destr(root);
}

void draw_tree(node * root, FILE* file)
{
    fprintf(file, "digraph {\n");
    draw_node(root, file);
    fprintf(file, "}");
}

void draw_node(node * root, FILE* file)
{
    fprintf(file, "\t\"%p\" ", root);

    if(root->type == 0)
    {
        switch ((int) root->value)
        {
        case 0:
            fprintf(file, "[label = \"%s\", color = \"red\"]\n", "+");
            break;
        case 1:
            fprintf(file, "[label = \"%s\", color = \"red\"]\n", "-");
            break;
        case 2:
            fprintf(file, "[label = \"%s\", color = \"red\"]\n", "*");
            break;
        case 3:
            fprintf(file, "[label = \"%s\", color = \"red\"]\n", "/");
            break;
        default:
            break;
        }
    }
    else if(root->type == 1)
    {
        fprintf(file, "[label = \"%lg\", color = \"blue\"]\n", root->value);
    }

    
    // fprintf(file, "{type: %d, value: %lg", root->type, root->value);

    if (root->yes != NULL)
    {
        fprintf(file, "\t\"%p\" -> \"%p\"\n", root, root->yes);
        draw_node(root->yes, file);
    }
    if (root->no != NULL)
    {
        fprintf(file, "\t\"%p\" -> \"%p\"\n", root, root->no);
        draw_node(root->no, file);
    }
}


void save_tree(node * root, FILE* file)
{
    if(root->type == 0)
    {
        switch ((int) root->value)
        {
        case 0:
            fprintf(file, "{%s", "+");
            break;
        case 1:
            fprintf(file, "{%s", "-");
            break;
        case 2:
            fprintf(file, "{%s", "*");
            break;
        case 3:
            fprintf(file, "{%s", "/");
            break;
        default:
            break;
        }
    }
    else if(root->type == 1)
    {
        fprintf(file, "{%lg", root->value);
    }
    // fprintf(file, "{type: %d, value: %lg", root->type, root->value);
    //
    //

    if (root->yes != NULL)
        save_tree(root->yes, file);
    if (root->no != NULL)
        save_tree(root->no, file);

    
    fprintf(file, "}");
}

void scan_tree(node * root, FILE* file)
{
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    printf("string size: %d\n", size);

    char* string = (char*) calloc((size + 1), sizeof(char));

    fread(string, sizeof(char), size, file);

    size_t pos = 0;

    fill_tree(root, string, &pos);
}

void fill_tree(node * root, char * string, size_t * pos)
{
    node * insert_node_yes = (node *) calloc(1, sizeof(node));
    node * insert_node_no = (node *) calloc(1, sizeof(node));

    size_t cnt = 1;

    char content[100] = {};

    while (string[*pos] != '{' && string[*pos] != '\0')
    {
        (*pos)++;
    }
    
    if (string[*pos] == '{')
    {
        while (string[*pos + cnt] != '}' && string[*pos + cnt] != '{')
        {
            cnt++;
        }

        strncpy(content, string + *pos + 1, cnt - 1);    
        printf("%s(%d); ", content, string[*pos + cnt]);

        if (isdigit(content[0]))
        {
            root->type = 1;
            root->value = strtol(content, NULL, 10);
        }
        else
        {
            if (!strcmp(content, "+"))
            {
                root->type = 0;
                root->value = 0;
            }
            else if (!strcmp(content, "-"))
            {
                root->type = 0;
                root->value = 1;
            }
            else if (!strcmp(content, "*"))
            {
                root->type = 0;
                root->value = 2;
            }
            else if (!strcmp(content, "/"))
            {
                root->type = 0;
                root->value = 3;
            }
        }
        
        *pos += cnt;

        if (string[*pos] == '{')
        {
            if (string[*pos + 1] == '}')
            {
                *pos += 3;
            }
            else 
            {
                root->no = insert_node_no;
                root->yes = insert_node_yes;

                fill_tree(root->yes, string, pos);
                fill_tree(root->no, string, pos);
            }
        }
    }
}

void tree_destr(node * root)
{
    if (root->yes != NULL)
        tree_destr(root->yes);
    if (root->no != NULL)
        tree_destr(root->no);


    free(root);
}
