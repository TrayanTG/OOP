#include<iostream>
#include<cstring>
#include<fstream>
#include<exception>
using namespace std;

const char *DIR_SUBJECTS = "subjects.txt";
const char *DIR_STUDENTS = "students.txt";

const size_t MAX_SUBJECTS_COUNT = 100;
const size_t MAX_SUBJECT_LENGTH = 64;
const size_t MAX_NAME_LENGTH = 128;
const size_t MAX_STUDENTS_COUNT = 100;
const size_t MAX_DIR_LENGTH = MAX_NAME_LENGTH + strlen(".txt");

struct Student
{
private: /// Can ignore this line for now
    size_t separatorIndex; /// The index of the ' ' symbol in the name

public: /// Can ignore this line for now
    int fn;
    char name[MAX_NAME_LENGTH];
    char subjects[MAX_SUBJECTS_COUNT][MAX_SUBJECT_LENGTH];
    size_t subjectsCount;

    Student() {}
    Student(const char *fName, const char *lName, int fn_);
    Student(ifstream &iFile); /// Constructs Student via file

    const char *getDir()const; /// Can ignore the const in the end for now (but NOT the one in the beginning!!)

    void print()const; /// Can ignore this const in the end for now
    bool addSubject(const char *newSubject);
};

void loadSubjects(ifstream &iFile, char (*subjects)[MAX_SUBJECT_LENGTH], size_t &subjectsCount);
void loadStudents(ifstream &iFile, Student *students, size_t &studentsCount);
Student loadStudent(const char *dir);

int main()
{
    char subjects[MAX_SUBJECTS_COUNT][MAX_SUBJECT_LENGTH];
    size_t subjectsCount = 0;

    /// ----------------------------- LOAD SUBJECTS -----------------------------
    ifstream iFile(DIR_SUBJECTS);
    if(!iFile)
    {
        cerr<<DIR_SUBJECTS<<" failed to open!\n";
        return 0;
    }
    loadSubjects(iFile, subjects, subjectsCount);
    iFile.close();
    /// -------------------------------------------------------------------------


    Student students[MAX_STUDENTS_COUNT];
    size_t studentsCount = 0;

    /// ----------------------------- LOAD STUDENTS -----------------------------
    iFile.open(DIR_STUDENTS);
    if(!iFile)
    {
        cerr<<DIR_STUDENTS<<" failed to open!\n";
        return 0;
    }
    try{
        loadStudents(iFile, students, studentsCount);
    }catch(exception &e)
    {
        cerr<<e.what()<<endl;
        return 0;
    }
    iFile.close();
    /// -------------------------------------------------------------------------

    cout<<"Data loaded!\n";

    while(true)
    {
        system("pause");
        system("cls");
        int t;
        cout<<"Options:\n";
        cout<<"1. Add new student\n";
        cout<<"2. Add subjects to a student already in the data\n";
        cout<<"3. Print student info\n";
        cout<<"4. Exit\n";
        cout<<"\nChoose option: ";
        cin>>t;
        system("cls");
        if(t == 4) return 0;
        else if(t == 3)
        {
            int fn;
            cout<<"Enter fn: ";
            cin>>fn;
            size_t i;
            for(i=0; i<studentsCount; i++)
             if(students[i].fn == fn)
             {
                 students[i].print();
                 break;
             }
            if(i<studentsCount) continue;
            cout<<"Invalid fn!\n";
        }
        else if(t == 2)
        {
            int fn;
            cout<<"Enter fn: ";
            cin>>fn;
            size_t i;
            for(i=0; i<studentsCount; i++)
             if(students[i].fn == fn)
             {
                 char newSubject[MAX_SUBJECT_LENGTH];
                 cout<<"Subject: ";
                 cin>>newSubject;

                 size_t j;
                 for(j=0; j<subjectsCount; j++)
                  if(!strcmp(newSubject, subjects[j]))
                   break;

                 if(j >= subjectsCount)
                 {
                     cout<<"Invalid subject!\n";
                     break;
                 }
                 try{
                     if(students[i].addSubject(newSubject)) cout<<"Subject added!\n";
                     else cout<<"Student already has this subject!\n";
                 }catch(exception &e)
                 {
                     cerr<<e.what();
                     return 0;
                 }
                 break;
             }
            if(i<studentsCount) continue;
            cout<<"Invalid fn!\n";
        }
        else if(t == 1)
        {
            char fName[MAX_NAME_LENGTH];
            char lName[MAX_NAME_LENGTH];
            int fn;
            cout<<"Enter first name: ";
            cin>>fName;
            cout<<"Enter last name: ";
            cin>>lName;
            cout<<"Enter fn: ";
            cin>>fn;

            size_t i;
            for(i=0; i<studentsCount; i++)
             if(fn == students[i].fn)
              break;

            if(i<studentsCount)
            {
                cout<<"Student with that fn already exists!\n";
                continue;
            }

            try
            {
                students[studentsCount] = Student(fName, lName, fn);
            }catch(exception &e)
            {
                cerr<<e.what()<<endl;
                return 0;
            }
            studentsCount++;
            cout<<"Student added!\n";
        }
        else cout<<"Invalid option!\n";
    }

    return 0;
}




Student::Student(const char *fName, const char *lName, int fn_)
{
    strcpy(name, fName);
    separatorIndex = strlen(name);
    name[separatorIndex] = ' ';
    name[separatorIndex+1] = 0; // or '\0';
    strcat(name, lName);
    fn = fn_;
    subjectsCount = 0;

    ofstream oFile(DIR_STUDENTS, ios::app);
    if(!oFile) throw runtime_error("Failed to open DIR_STUDENTS!");
    oFile<<name<<' '<<fn<<endl;
    oFile.close();

    char dir[MAX_DIR_LENGTH];
    strcpy(dir, name);
    dir[0] += ('a'-'A');
    dir[separatorIndex]='_';
    dir[separatorIndex+1] += ('a'-'A');
    oFile.open(strcat(dir, ".txt"), ios::app);
    oFile<<name<<' '<<fn<<endl;
}

Student::Student(ifstream &iFile)
{
    iFile>>name;
    separatorIndex = strlen(name);
    name[separatorIndex] = ' ';
    name[separatorIndex+1] = 0; // or '\0';
    char tempLastName[MAX_NAME_LENGTH];
    iFile>>tempLastName;
    strcat(name, tempLastName);
    iFile>>fn;

    subjectsCount = 0;
    while(!iFile.eof())
    {
        char subject[MAX_SUBJECT_LENGTH];
        iFile>>subject;
        if(iFile.eof()) break; /// If it's empty then nothing left to read
        strcpy(subjects[subjectsCount], subject);
        subjectsCount++;
    }
}

void Student::print()const
{
    cout<<name<<' '<<fn<<": ";
    if(!subjectsCount)
    {
        cout<<"non\n";
        return;
    }
    for(size_t i=0; i<subjectsCount-1; i++) cout<<subjects[i]<<", ";
    cout<<subjects[subjectsCount-1]<<endl;
}

bool Student::addSubject(const char *newSubject)
{
    for(size_t i=0; i<subjectsCount; i++)
     if(!strcmp(newSubject, subjects[i]))
      return false;

    strcpy(subjects[subjectsCount], newSubject);
    subjectsCount++;

    char dir[MAX_DIR_LENGTH];
    strcpy(dir, name);
    dir[0] += ('a'-'A');
    dir[separatorIndex]='_';
    dir[separatorIndex+1] += ('a'-'A');
    ofstream oFile(strcat(dir, ".txt"), ios::app);
    if(!oFile) throw runtime_error("Failed to open student file to add new subject!");
    oFile<<newSubject<<endl;

    return true;
}

void loadSubjects(ifstream &iFile, char (*subjects)[MAX_SUBJECT_LENGTH], size_t &subjectsCount)
{
    while(!iFile.eof())
    {
        iFile>>subjects[subjectsCount];
        if(!iFile.eof()) subjectsCount++; /// If eof is not reached then we have read a new subject, so we increase the subjects count
    }
}

void loadStudents(ifstream &iFile, Student *students, size_t &studentsCount)
{
    while(!iFile.eof())
    {
        char fName[MAX_NAME_LENGTH];
        char lName[MAX_NAME_LENGTH];
        int fn;

        iFile>>fName>>lName>>fn;
        fName[0] += ('a'-'A');
        lName[0] += ('a'-'A');

        if(iFile.eof()) break;
        students[studentsCount] = loadStudent(strcat(strcat(strcat(fName,"_"), lName),".txt")); /// If failed to load, throws an exception
        studentsCount++;
    }
}

Student loadStudent(const char *dir)
{
    ifstream iFile(dir);
    if(!iFile) throw runtime_error("Failed to open student dir!");
    return Student(iFile);
}
