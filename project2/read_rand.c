#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
// 필요하면 header file 추가 가능


#define SUFFLE_NUM 100000 // 이 값은 마음대로 수정할 수 있음
#define STUDENT_RECORD_SIZE 100

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
// 필요한 함수가 있으면 더 추가할 수 있음

//
// argv[1]: 레코드 파일명
//
int main(int argc, char **argv)
{
	char student[STUDENT_RECORD_SIZE];
	off_t fileSize;
	int fd;
	int i;
	struct timeval begin_t, end_t, interval_t;
	suseconds_t runtime;
	int *read_order_list;
	int num_of_records;

	// 'read_order_list'를 이용하여 표준 입력으로 받은 레코드 파일로부터 레코드를 random 하게 읽어들이고, 
        // 이때 걸리는 시간을 측정하는 코드 구현함

	if(argc != 2) { // 프로그램에 전달된 인자의 개수가 잘못되었는지 확인
		fprintf(stderr, "usage : %s <file>\n", argv[0]);
		exit(1);
	}

	if((fd = open(argv[1], O_RDONLY)) < 0) { // 파일 open
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	if((fileSize = lseek(fd, 0, SEEK_END)) < 0) { // 파일의 크기를 구한다
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	num_of_records = (int)(fileSize / STUDENT_RECORD_SIZE); // 파일에 저장되어있는 레코드의 개수를 계산
	read_order_list = (int *)malloc(num_of_records * sizeof(int));
	
	// 아래 함수를 실행하면 'read_order_list' 배열에 추후 랜덤하게 읽어야 할 레코드 번호들이 순서대로 나열되어 저장됨
        // 'num_of_records'는 레코드 파일에 저장되어 있는 전체 레코드의 수를 의미함
	GenRecordSequence(read_order_list, num_of_records);

	lseek(fd, 0, SEEK_SET);
	gettimeofday(&begin_t, NULL); // 시간 측정 시작
	for(i = 0; i < num_of_records; ++i) { // 레코드를 랜덤하게 읽어 들인다
		lseek(fd, read_order_list[i] * STUDENT_RECORD_SIZE, SEEK_SET);
		if (read(fd, &student, STUDENT_RECORD_SIZE) <= 0)
			break;
	}
	gettimeofday(&end_t, NULL); // 시간 측정 종료
	
	// read하는데 걸린 시간 계산
	interval_t.tv_sec = end_t.tv_sec - begin_t.tv_sec;
	interval_t.tv_usec = end_t.tv_usec - begin_t.tv_usec;
	if(interval_t.tv_usec < 0) {
		interval_t.tv_usec += 1000000;
		interval_t.tv_sec -= 1;
	}
	runtime = interval_t.tv_sec * 1000000 + interval_t.tv_usec;

	printf("#records: %d timecost: %ld us\n",num_of_records, runtime); // 결과 출력

	free(read_order_list);
	exit(0);

	return 0;
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for(i=0; i<n; i++)
	{
		list[i] = i;
	}
	
	for(i=0; i<SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}

	return;
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

	return;
}
