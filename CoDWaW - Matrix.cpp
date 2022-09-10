#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <iomanip>


#define PROCESS_NAME "CoDWaW.exe"
#define ZOMBIE_BASE_ADDRESS 0x18E745C
#define ZOMBIE_OFFSET 0x110
#define ZOMBIE_X_POSITION_OFFSET 0x0
#define ZOMBIE_Y_POSITION_OFFSET 0x4
#define ZOMBIE_Z_POSITION_OFFSET 0x8
#define ZOMBIE_DEAD_OFFSET 0xF0
#define ZOMBIE_DEAD_VALUE 0
#define TOTAL_NUMBER_OF_ZOMBIES 25
#define PLAYER_X_POSITION 0x18ED088
#define PLAYER_Y_POSITION 0x18ED08C
#define PLAYER_Z_POSITION 0x18ED090
#define POLL_RATE 100

DWORD GetProcessIDByProcessName(const char* process_name_)
{
	PROCESSENTRY32 process_entry = {sizeof(PROCESSENTRY32)};
	HANDLE processes_snapshot;
	DWORD process_id = 0;

	// Procurar pelo nome do processo
	processes_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Process32First(processes_snapshot, &process_entry))
	{
		do
		{
			if (!strcmp(process_entry.szExeFile, process_name_))
			{
				process_id = process_entry.th32ProcessID;
				break;
			}
		} while (Process32Next(processes_snapshot, &process_entry));
	}

	// Limpando Recursos
	CloseHandle(processes_snapshot);


	return process_id;
}

int main(int argc_, char** argv_)
{
	//Pegando o jogo
	std::cout << "Esperando o jogo ser agarrado." << std::endl;
	DWORD process = NULL;
	while (process == NULL)
	{
		process = GetProcessIDByProcessName(PROCESS_NAME);
	}
	HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process);


	//Checando se pegou o id do processo certo
	if (process_handle != NULL)
	{
		std::cout << "Pegamos o Jogo." << std::endl;
		while (true)
		{
			//limpar display de tela pleyer e zombie info
			system("cls");

			//pegando coordenadas xyz
			float x, y, z;
			ReadProcessMemory(process_handle, (LPVOID)(PLAYER_X_POSITION), &x, sizeof(float), NULL);
			ReadProcessMemory(process_handle, (LPVOID)(PLAYER_Y_POSITION), &y, sizeof(float), NULL);
			ReadProcessMemory(process_handle, (LPVOID)(PLAYER_Z_POSITION), &z, sizeof(float), NULL);


			std::cout << "Jogador " << std::setw(2) << "" << ": " << std::setw(9) << x << ", " << std::setw(9) << y << ", " << std::setw(9) << z << std::endl;

			// um loop por todos os zombies e mostrar info dos zombies
			for (int zombie_index = 0; zombie_index < TOTAL_NUMBER_OF_ZOMBIES; zombie_index++)
			{
				int dead;

				//pegando xyz zombie cordenadas and indicardor de morto
				ReadProcessMemory(process_handle, (LPVOID)(ZOMBIE_BASE_ADDRESS + zombie_index * ZOMBIE_OFFSET + ZOMBIE_X_POSITION_OFFSET), &x, sizeof(float), NULL);
				ReadProcessMemory(process_handle, (LPVOID)(ZOMBIE_BASE_ADDRESS + zombie_index * ZOMBIE_OFFSET + ZOMBIE_Y_POSITION_OFFSET), &y, sizeof(float), NULL);
				ReadProcessMemory(process_handle, (LPVOID)(ZOMBIE_BASE_ADDRESS + zombie_index * ZOMBIE_OFFSET + ZOMBIE_Z_POSITION_OFFSET), &z, sizeof(float), NULL);
				ReadProcessMemory(process_handle, (LPVOID)(ZOMBIE_BASE_ADDRESS + zombie_index * ZOMBIE_OFFSET + ZOMBIE_DEAD_OFFSET), &dead, sizeof(int), NULL);

				//Mostrar zombie info
				std::cout << "Zumbi " << std::setw(2) << zombie_index + 1 << ": " << std::setw(9) << x << ", " << std::setw(9) << y << ", " << std::setw(9) << z << " " << (dead == ZOMBIE_DEAD_VALUE ? "morto" : "vivo") << std::endl;
			}

			//delay depois de atualziar a tela novamente
			Sleep(POLL_RATE);
		}
	}
}