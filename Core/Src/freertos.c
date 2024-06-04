/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint8_t op_data_RX[20];
uint8_t op_data_TX[20];
uint8_t cam_data_TX[20];
uint8_t cam_data_RX[20];
uint8_t br_inc[9]={0x02,0x10,0x3F,0xE0,0x00,0x03,0x10,0x00,0x03};
uint8_t br_dec[9]={0x02,0x10,0x3F,0xE0,0x00,0x03,0x20,0x00,0x03};
uint8_t awb[9]	={0x02,0x10,0x3F,0xE0,0x00,0x15,0x00,0x01,0x03};

uint8_t br_inc_old;
uint8_t br_dec_old;
uint8_t awb_old;

int8_t pos_f,pos_z;
int8_t dir_f,dir_z;
uint16_t contrast;
uint16_t old_contrast;
uint16_t contrast1;
uint16_t contrast2;
uint16_t contrast3;
uint16_t contrast4;

uint16_t data[2][2000];
uint16_t i;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for communication */
osThreadId_t communicationHandle;
const osThreadAttr_t communication_attributes = {
  .name = "communication",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow1,
};
/* Definitions for buttonsRead */
osThreadId_t buttonsReadHandle;
const osThreadAttr_t buttonsRead_attributes = {
  .name = "buttonsRead",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for received */
osEventFlagsId_t receivedHandle;
const osEventFlagsAttr_t received_attributes = {
  .name = "received"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	osEventFlagsSet(receivedHandle, 0x01);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	//osEventFlagsSet(receivedHandle, 0x01);
}

void f_zero()
{
	pos_f=32;
	HAL_GPIO_WritePin(M2R_GPIO_Port,M2R_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_SET);
	while(HAL_GPIO_ReadPin(END1_GPIO_Port, END1_Pin) !=  GPIO_PIN_RESET);
	HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_RESET);
	osDelay(50);
	pos_f=0;
}

void z_zero()
{
	pos_z=48;
	HAL_GPIO_WritePin(M1F_GPIO_Port,M1F_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(M1R_GPIO_Port,M1R_Pin,GPIO_PIN_SET);
	while(HAL_GPIO_ReadPin(END2_GPIO_Port, END2_Pin) !=  GPIO_PIN_RESET);
	HAL_GPIO_WritePin(M1R_GPIO_Port,M1R_Pin,GPIO_PIN_RESET);
	osDelay(50);
	pos_z=0;
	dir_z = 1;
	HAL_GPIO_WritePin(M1F_GPIO_Port,M1F_Pin,GPIO_PIN_SET);
	while(pos_z <5);
	//osDelay(120);
	HAL_GPIO_WritePin(M1F_GPIO_Port,M1F_Pin,GPIO_PIN_RESET);
}
void stop_f()
{

		HAL_GPIO_WritePin(M2R_GPIO_Port,M2R_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_SET);
		osDelay(2);
		HAL_GPIO_WritePin(M2R_GPIO_Port,M2R_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_RESET);
}

void autofocus()
{
	i=0;
	old_contrast = contrast;
	while (pos_f>0)
	{
		dir_f=0;
		HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_SET);
		osDelay(20);
		stop_f();
	}
	HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_RESET);

	while ((pos_f<=31)&&((pos_f+pos_z)<=64)&&(old_contrast <= (contrast+1))){

		data[0][i]=contrast;
		data[1][i]=pos_f;
		i++;
		dir_f=1;
		old_contrast = contrast;
		HAL_GPIO_WritePin(M2R_GPIO_Port,M2R_Pin,GPIO_PIN_SET);
		osDelay(20);
		stop_f();
		HAL_GPIO_WritePin(M2R_GPIO_Port,M2R_Pin,GPIO_PIN_RESET);
		if (i>200) return;

	}

	while ((pos_f>=2)&&(old_contrast <= contrast)){

		data[0][i]=contrast;
		data[1][i]=pos_f;
		i++;
		dir_f=0;
		old_contrast = contrast;
		HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_SET);
		osDelay(20);
		stop_f();
		HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_RESET);
		if (i>400) return;
	}
	int imax = 0;
	for (int j=0; j<i; j++)
	{
		if (data[0][imax]<=data[0][j]) imax = j;
	}

	i=0;
	while (pos_f >= (data[1][imax])){
			dir_f=0;
			old_contrast = contrast;
			HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_SET);
			osDelay(20);
			stop_f();
			HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_RESET);
		//	if (i>150) return;
			i++;
		}
	i=0;
	while ((contrast < (0.98*data[0][imax]))&&(pos_f>=2)&&(pos_f<=31)&&((pos_f+pos_z)<=64)){
		dir_f=0;
		old_contrast = contrast;
		HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_SET);
		osDelay(20);
		stop_f();
		HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_RESET);
		osDelay(150);
		if (i>150) return;
		i++;

	}




	/*
	i=0;
	contrast1 = contrast;
	do
	{
		data[i]=contrast;
		i++;
		dir_f=1;
		old_contrast = contrast;
		HAL_GPIO_WritePin(M2R_GPIO_Port,M2R_Pin,GPIO_PIN_SET);
		osDelay(100);
		data[i]=contrast;
		i++;
		stop_f();
		data[i]=contrast;
		i++;
		HAL_GPIO_WritePin(M2R_GPIO_Port,M2R_Pin,GPIO_PIN_RESET);

	}while ((old_contrast <= contrast)&&(pos_f<=30));

	contrast2 = contrast;

	data[i]=0xff;
	i+=4;


	do
	{
		data[i]=contrast;
		i++;
		old_contrast = contrast;
		dir_f=0;
		HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_SET);
		osDelay(100);
		data[i]=contrast;
		i++;
		stop_f();
		data[i]=contrast;
		i++;
		HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_RESET);
	}while ((old_contrast <= contrast)&&(pos_f>=2));
	contrast3 = contrast;


	HAL_GPIO_WritePin(M2R_GPIO_Port,M2R_Pin,GPIO_PIN_SET);
	dir_f=1;
	osDelay(10);
	HAL_GPIO_WritePin(M2R_GPIO_Port,M2R_Pin,GPIO_PIN_RESET);
	contrast4 = contrast;*/

}

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void CommTask(void *argument);
void BtnReadTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of communication */
  communicationHandle = osThreadNew(CommTask, NULL, &communication_attributes);

  /* creation of buttonsRead */
  buttonsReadHandle = osThreadNew(BtnReadTask, NULL, &buttonsRead_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of received */
  receivedHandle = osEventFlagsNew(&received_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	f_zero();
	z_zero();
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_CommTask */
/**
* @brief Function implementing the communication thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CommTask */
void CommTask(void *argument)
{
  /* USER CODE BEGIN CommTask */
	osDelay(500);
  /* Infinite loop */
  for(;;)
  {

	  	HAL_UART_AbortReceive(&huart1);
	  	while(HAL_UART_Receive_DMA(&huart1, op_data_RX, 20)!=HAL_OK);
	  	osEventFlagsWait(receivedHandle, 0x01, osFlagsWaitAny, 500); //Ожидание флага приема данных
	  	uint8_t contrast_temp;
	  	contrast_temp = (op_data_RX[3]+(op_data_RX[2]<<8))/100;
	  	if (contrast_temp<100) contrast=contrast_temp;
	  	for (int i = 0; i<10; i++) cam_data_TX[i] = op_data_RX[i+8];

	  	if (cam_data_TX[0] != 0)
	  	{
	  		if (cam_data_TX[1] == 0x10) HAL_UART_Transmit(&huart3,cam_data_TX,9,1000);
	  		else HAL_UART_Transmit(&huart3,cam_data_TX,4,1000);
	  	}
	  	HAL_UART_Transmit_DMA(&huart1,op_data_TX,20);


    osDelay(1);
  }
  /* USER CODE END CommTask */
}

/* USER CODE BEGIN Header_BtnReadTask */
/**
* @brief Function implementing the buttonsRead thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_BtnReadTask */
void BtnReadTask(void *argument)
{
  /* USER CODE BEGIN BtnReadTask */
	osDelay(500);
  /* Infinite loop */
  for(;;)
  {

	  op_data_TX[0]=0xfe;
	  op_data_TX[1]++;
	  op_data_TX[3]=0;
	  op_data_TX[4]=0;
	  op_data_TX[5]=0;
	  HAL_GPIO_WritePin(M1R_GPIO_Port,M1R_Pin,GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(M1F_GPIO_Port,M1F_Pin,GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(M2R_GPIO_Port,M2R_Pin,GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_RESET);

	  /*кнопки*/
	  //+focus
	  if (HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin) ==  GPIO_PIN_RESET){
		  if ((pos_f<=31)&&((pos_f+pos_z)<=64)){
			  op_data_TX[3] += 0b00000001;
			  dir_f = 1;
			  HAL_GPIO_WritePin(M2R_GPIO_Port,M2R_Pin,GPIO_PIN_SET);
			  osDelay(10);
			  HAL_GPIO_WritePin(M2R_GPIO_Port,M2R_Pin,GPIO_PIN_RESET);

		  }
	  }
	  //-focus
	  if (HAL_GPIO_ReadPin(BTN9_GPIO_Port, BTN9_Pin) ==  GPIO_PIN_RESET){
		  if(pos_f>0){
			  op_data_TX[3] += 0b00000010;
			  dir_f = 0;
			  HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_SET);
			  osDelay(10);
			  HAL_GPIO_WritePin(M2F_GPIO_Port,M2F_Pin,GPIO_PIN_RESET);


		  }

	  }
	  //autofocus
	  if (HAL_GPIO_ReadPin(BTN4_GPIO_Port, BTN4_Pin) ==  GPIO_PIN_RESET){
		  op_data_TX[3] += 0b00000100;
		  autofocus();
	  }

	  //+zoom
	  if (HAL_GPIO_ReadPin(BTN2_GPIO_Port, BTN2_Pin) ==  GPIO_PIN_RESET){
		  if ((pos_z<=47)&&((pos_f+pos_z)<=64)){
			  op_data_TX[3] += 0b00001000;
			  dir_z = 1;
			  HAL_GPIO_WritePin(M1F_GPIO_Port,M1F_Pin,GPIO_PIN_SET);
		  }

	  }
	  //-zoom
	  if (HAL_GPIO_ReadPin(BTN8_GPIO_Port, BTN8_Pin) ==  GPIO_PIN_RESET){
		  if (pos_z>0){
			  op_data_TX[3] += 0b00010000;
			  dir_z=0;
			  HAL_GPIO_WritePin(M1R_GPIO_Port,M1R_Pin,GPIO_PIN_SET);
		  }
	  }
	  //L
	  if (HAL_GPIO_ReadPin(BTN7_GPIO_Port, BTN7_Pin) ==  GPIO_PIN_RESET){
		  op_data_TX[3] += 0b00100000;
		  if (br_dec_old==0)
		  {
			  HAL_UART_Transmit(&huart3,br_dec,9,1000);
			  osDelay(50);
		  }
		  br_dec_old = 1;
	  }
	  else br_dec_old = 0;
	  //R
	  if (HAL_GPIO_ReadPin(BTN3_GPIO_Port, BTN3_Pin) ==  GPIO_PIN_RESET){
		  op_data_TX[3] += 0b01000000;
		  if (br_inc_old==0)
		  {
			  HAL_UART_Transmit(&huart3,br_inc,9,1000);
			  osDelay(50);
		  }
		  br_inc_old =1;
	  }
	  else br_inc_old = 0;
	  //AWB
	  if (HAL_GPIO_ReadPin(BTN5_GPIO_Port, BTN5_Pin) ==  GPIO_PIN_RESET){
		  op_data_TX[3] += 0b10000000;
		  if (br_inc_old==0)
		  {
			  HAL_UART_Transmit(&huart3,awb,9,1000);
			  osDelay(50);
		  }
		  awb_old = 1;
	  }
	  else awb_old = 0;
	  //Snapshot
	  if (HAL_GPIO_ReadPin(BTN6_GPIO_Port, BTN6_Pin) ==  GPIO_PIN_RESET){
		  op_data_TX[4] += 0b00000001;
	  }





	  /* датчики положения оптические*/
	  if (HAL_GPIO_ReadPin(ENC1_GPIO_Port, ENC1_Pin) ==  GPIO_PIN_RESET){
		  op_data_TX[5] += 0b00000001;
	  }
	  if (HAL_GPIO_ReadPin(ENC2_GPIO_Port, ENC2_Pin) ==  GPIO_PIN_RESET){
		  op_data_TX[5] += 0b00000010;
	  }
	  if (HAL_GPIO_ReadPin(END1_GPIO_Port, END1_Pin) ==  GPIO_PIN_RESET){
	  	  op_data_TX[5] += 0b00000100;
	  }
	  if (HAL_GPIO_ReadPin(END2_GPIO_Port, END2_Pin) ==  GPIO_PIN_RESET){
	  	  op_data_TX[5] += 0b00001000;
	  }

	  osDelay(10);
  }
  /* USER CODE END BtnReadTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

