// Framework for Create, Read, Update, Delete operations

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FIELDS "input_fields.cfg"
#define MENU_OPTIONS "read_menu.cfg"
#define DATA_FILE "user_details.dat"
#define SIZE 50
#define MAX_FIELDS 1
0

void create();
void show_all();
void update();
void delete_record();

int get_field_count();
void load_fields_into_array(FILE *, char field_names[][SIZE], int);
void remove_new_line(char *);
void open_field_file();
void close_field_file();
int find_record(char*);
void print_account_not_found();
void print_invalid();
int loading_files(char field_names[][SIZE]);
void read_key_field(char field_names[][SIZE]);


FILE *fp_menu, *fp_main_data, *fp_fields;
int counter;
int field_count;
char check_key_field[SIZE];
char field_value[SIZE];
int choice;
char status;
char field_names[MAX_FIELDS][SIZE];


int main()
{
	char menu_options[SIZE];
	while (1)
	{
		fp_menu = fopen(MENU_OPTIONS, "r");
		if (!fp_menu)
		{
			printf("Unable to open menu. ");
			return 0;
		}
		printf("\n\n =========MENU=========\n\n");
		while (fgets(menu_options, SIZE, fp_menu))
		{
			remove_new_line(menu_options);
			printf("%s\n", menu_options);
		}
		fclose(fp_menu);
		scanf("%d", &choice);
		getchar();
		switch(choice)
		{
		case 1:
			create();
			break;
		case 2:
			show_all();
			break;
		case 3:
			update();
			break;
		case 4:
			delete_record();
			break;
		case 5:
			exit(0);

		default:
			print_invalid();
		}
	}
	
}

int get_field_count()
{
	char field_name[SIZE];
	int input_field_count = 0;
	open_field_file();
	if (!fp_fields)
	{
		return 0;
	}
	while (fgets(field_name, SIZE, fp_fields))
	{
		input_field_count++;
	}
	close_field_file();
	return input_field_count;
}

void load_fields_into_array(FILE *fp_fields, char field_names[][SIZE], int field_count)
{
	for (counter = 0; counter < field_count; counter++)
	{
		fgets(field_names[counter], SIZE, fp_fields);
		remove_new_line(field_names[counter]);
	}
}




void create()
{
	field_count = loading_files(field_names);
	char user_details[field_count][SIZE];
	fp_main_data = fopen(DATA_FILE, "ab");
	for (counter = 0; counter < field_count; counter++)
	{
		printf("Enter %s: ", field_names[counter]);
		fgets(user_details[counter], SIZE, stdin);
		remove_new_line(user_details[counter]);
		fwrite(user_details[counter], SIZE, 1, fp_main_data);
	}
	status = 'A';
	fwrite(&status, sizeof(char), 1, fp_main_data);
	fclose(fp_main_data);
	printf("Record created successfully. ");
}

void show_all()
{
    field_count = loading_files(field_names);
	fp_main_data = fopen(DATA_FILE, "rb");
	if (!fp_main_data)
	{
		printf("No records found. ");
		return;
	}
	printf("\n========ALL RECORDS=========\n");
	while (1)
	{

		for (counter = 0; counter < field_count; counter++)
		{
			if (fread(field_value, SIZE, 1, fp_main_data) != 1)
			{
				fclose(fp_main_data);
				return;
			}

			printf("\n%s: %s\n",field_names[counter], field_value);
		}

		fread(&status, sizeof(char), 1, fp_main_data);

		if (status == 'A')
		{
			printf("----------------------------\n");
		}
		else
		{
			printf("------(Deleted Record)-------\n");
		}
	}
	
}

void update()
{
	char new_value[SIZE];
	field_count = loading_files(field_names);
    read_key_field(field_names);
    if (find_record(check_key_field))
    {
    	printf("1. Update %s\n2. Update %s\n", field_names[1], field_names[2]);
    	printf("Enter choice: ");
    	scanf("%d", &choice);
    	getchar();

    	if (choice < 1 || choice > field_count - 1)
    	{
    		print_invalid();
    		fclose(fp_main_data);
    		return;
    	}
    	printf("Enter new %s: ", field_names[choice]);
    	fgets(new_value, SIZE, stdin);
    	remove_new_line(new_value);
    	fseek(fp_main_data, SIZE * choice, SEEK_CUR);
    	fwrite(new_value, SIZE, 1, fp_main_data);
    	fclose(fp_main_data);
    	printf("Record updated successfully. ");
    }
    
}

int find_record(char* check_key_field)
{
	fp_main_data = fopen(DATA_FILE, "rb+");
	if (!fp_main_data)
	{
		return 0;
	}
	while (fread(field_value, SIZE, 1, fp_main_data))
	{
		if (strcmp(field_value, check_key_field) == 0 )
		{
			fseek(fp_main_data, SIZE * (field_count - 1), SEEK_CUR);
			fread(&status, sizeof(char), 1, fp_main_data);
			if (status == 'A')
			{
				fseek(fp_main_data, -(SIZE * field_count + sizeof(char)), SEEK_CUR);
				return 1;
			}
		}
		else
		{
			fseek(fp_main_data, SIZE * (field_count - 1) + sizeof(char), SEEK_CUR);
		}
	}
	fclose(fp_main_data);
	print_account_not_found();
	return 0;
}


void delete_record()
{
	char confirmation;
    char status = 'D';
    field_count = loading_files(field_names);
    read_key_field(field_names);
    if (find_record(check_key_field))
    {
    	printf("You really want to delete record then press 'y'. ");
    	scanf("%c", &confirmation);
    	if (confirmation == 'y')
    	{
    		fseek(fp_main_data, SIZE * field_count, SEEK_CUR);
    		fwrite(&status, sizeof(char), 1, fp_main_data);
    		fclose(fp_main_data);
    		printf("Record deleted successfully.\n");
    	}
    }

    
}

void print_account_not_found()
{
	printf("Record not found. ");
}

void print_invalid()
{
	printf("Invalid option. ");
}

void read_key_field(char field_names[][SIZE])
{
    printf("Enter %s: ", field_names[0]);
    fgets(check_key_field, SIZE, stdin);
    remove_new_line(check_key_field);
}

void open_field_file()
{
	fp_fields = fopen(INPUT_FIELDS, "r");
}

void close_field_file()
{
	if(fp_fields)
	{
		fclose(fp_fields);
	}
	
}

void remove_new_line(char *text)
{
	int length = strlen(text);
	if (length > 0 && text[length - 1] == '\n')
	{
		text[length - 1] = '\0';
	}
}

int loading_files(char field_names[][SIZE])
{
    field_count = get_field_count();
	open_field_file();
    load_fields_into_array(fp_fields, field_names, field_count);
    close_field_file();
    return field_count;
}
