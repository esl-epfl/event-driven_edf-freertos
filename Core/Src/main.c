int main(void)
{
	launch_comm();
	init_hw();
	launch_ssAll();


	vTaskStartScheduler();

	while (1);

}
