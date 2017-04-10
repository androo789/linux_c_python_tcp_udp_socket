#include <stdio.h>
//#include <stdlib.h>
     main()
     {
          char *s="That's good news";  /*定义字符串指针并初始化*/
          int i=617;                    /*定义整型变量并初始化*/
          FILE *fp;                     /*定义文件指针*/
          fp=fopen("test.txt", "w");    /*建立一个文字文件只写*/
          fputs("Your score of TOEFLis", fp);/*向所建文件写入一串字符*/
          fputc(':', fp);               /*向所建文件写冒号:*/
          fprintf(fp, "%d/n", i);       /*向所建文件写一整型数*/
          fprintf(fp, "%s", s);         /*向所建文件写一字符串*/
          fclose(fp);                   /*关闭文件*/
     }
