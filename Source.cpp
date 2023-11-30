#include <windows.h>
#include "resource.h"
#include <cstdio>
#include <vector> 

struct Compra {
	const char* evento;
	int cantidad;
	int totalFinal;
};

std::vector<Compra> historialCompras;
const char* opcionesEventos[] = { "BTS", "Concierto de navidad", "Orquesta" };
static const int precios[] = { 4800, 950, 1200 };
Compra compraActual;

//Prototipos ventanas 
LRESULT CALLBACK vInicioSesion(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK vMenu(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK vEventosMes(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK vVentaBoletos(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK vMisCompras (HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, PSTR cmdLine, int cShow)
{

	HWND hInicioSesion = CreateDialog(hInst, MAKEINTRESOURCE(INICIOSESION), NULL, vInicioSesion); 
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	ShowWindow(hInicioSesion, cShow);

	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

//Ventana inicio de sesión 
const char usuario_predefinido[] = "Musica";
const char contrasena_predefinida[] = "evento123*";
LRESULT CALLBACK vInicioSesion(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_CLOSE:
	{
		int respuesta = MessageBox(hwnd, "¿Deseas cerrar el programa?", "Confirmacion", MB_YESNO | MB_ICONWARNING);
		if (respuesta == IDYES)
		{
			DestroyWindow(hwnd);
		}

	}break;

	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}break;

	case WM_COMMAND:
	{
		if (LOWORD(wParam) == BTN_INGRESAR && HIWORD(wParam) == BN_CLICKED) {
			HWND hUsuario = GetDlgItem(hwnd, TXT_USUARIO);
			int usuariolenght = GetWindowTextLength(hUsuario);
			char buffer[256];
			GetWindowText(hUsuario, buffer, sizeof(buffer));

			HWND hContrasena = GetDlgItem(hwnd, TXT_CONTRA);
			int contralenght = GetWindowTextLength(hContrasena);
			char buffer1[256];
			GetWindowText(hContrasena, buffer1, sizeof(buffer1));

			if (usuariolenght < 4) {
				MessageBox(hwnd, "El nombre de usuario debe tener mínimo 4 caracteres", "ADVERTENCIA", MB_OK | MB_ICONWARNING);
			}

			if (contralenght != 10 || strcmp(buffer1, contrasena_predefinida) != 0) {
				MessageBox(hwnd, "La contraseña debe ser 'progra123*'", "ADVERTENCIA", MB_OK | MB_ICONWARNING);
			}

			if (4 <= usuariolenght && contralenght == 10 && strcmp(buffer, usuario_predefinido) == 0) {
				EndDialog(hwnd, 0);
				HWND hMenu = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(MENU), hwnd, vMenu); 
				ShowWindow(hMenu, SW_SHOW);
			}
			else {
				MessageBox(hwnd, "Usuario o contraseña incorrectos", "ADVERTENCIA", MB_OK | MB_ICONWARNING);
			}
		}
	}
	}
	return FALSE;
}

//Ventana menú
LRESULT CALLBACK vMenu(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_COMMAND: {
		if (LOWORD(wParam) == BTN_EVENTOS && HIWORD(wParam) == BN_CLICKED)  //Cierra ventana menú y abre ventana eventos del mes
		{
			EndDialog(hwnd, 0);
			HWND hEventos = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(EVENTOSMES), hwnd, vEventosMes);  
			ShowWindow(hEventos, SW_SHOW); 
		}
		if (LOWORD(wParam) == BTN_VENTA && HIWORD(wParam) == BN_CLICKED) //Cierra ventana menú y abre ventana venta de boletos
		{
			EndDialog(hwnd, 0);
			HWND hVenta = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(VENTABOLETOS), hwnd, vVentaBoletos); 
			ShowWindow(hVenta, SW_SHOW); 
		}
		if (LOWORD(wParam) == BTN_COMPRAS && HIWORD(wParam) == BN_CLICKED) //Cierra ventana menú y abre ventana compras
		{
			EndDialog(hwnd, 0);
			HWND hCompras = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(MISCOMPRAS), hwnd, vMisCompras); 
			ShowWindow(hCompras, SW_SHOW); 
		}
		if (LOWORD(wParam) == BTN_SALIR && HIWORD(wParam) == BN_CLICKED) //Salir del programa
		{
			int respuesta = MessageBox(hwnd, "El programa esta por cerrarse, ¿deseas continuar?", "ADVERTENCIA", MB_YESNO | MB_ICONWARNING);
			if (respuesta == IDYES)
			{
				DestroyWindow(hwnd);
				PostQuitMessage(0);
			}
		}
	}
	}
	return FALSE;
}

//Ventana eventos del mes
LRESULT CALLBACK vEventosMes(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_INITDIALOG: { //Muestra los eventos en el listbox
		HWND hListBox = GetDlgItem(hwnd, LIST_EVENTOS);

		for (const char* evento : opcionesEventos) {
			SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)evento);
		}
	} break;

	case WM_COMMAND: //Regresa al menú
		if (LOWORD(wParam) == BTN_VOLVER && HIWORD(wParam) == BN_CLICKED) {     
			EndDialog(hwnd, 0); 
			HWND hMenu = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(MENU), hwnd, vMenu);
			ShowWindow(hMenu, SW_SHOW);
		}
		break;
	}

	return FALSE;
}

//Ventana para la compra de boletos
LRESULT CALLBACK vVentaBoletos(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static const int maxBoletos = 4;
	static const double iva = 0.16;
	static const double descuento = 0.10;

	int totalFinal = 0; 

	switch (msg)
	{
	case WM_INITDIALOG: {
		HWND hComboBoxEventos = GetDlgItem(hwnd, CB_EVENTO);
		for (int i = 0; i < 3; ++i) {
			SendMessageA(hComboBoxEventos, CB_ADDSTRING, 0, (LPARAM)opcionesEventos[i]);
		}

		HWND hComboBoxCantidad = GetDlgItem(hwnd, CB_CANTIDAD);
		for (int i = 1; i <= maxBoletos; ++i) {
			char buffer[4];
			_itoa_s(i, buffer, 10);
			SendMessageA(hComboBoxCantidad, CB_ADDSTRING, 0, (LPARAM)buffer);
		}

		HWND hComboBoxPromocion = GetDlgItem(hwnd, CB_PROMO);
		SendMessageA(hComboBoxPromocion, CB_ADDSTRING, 0, (LPARAM)"2x1");
		SendMessageA(hComboBoxPromocion, CB_ADDSTRING, 0, (LPARAM)"10% Descuento");

	} break;

	case WM_COMMAND: {
		if (HIWORD(wParam) == CBN_SELCHANGE) {

			// Obtener opción del ComboBox de eventos
			HWND hComboBoxEventos = GetDlgItem(hwnd, CB_EVENTO);
			int eventoSeleccionado = SendMessageA(hComboBoxEventos, CB_GETCURSEL, 0, 0);

			// Obtener cantidad del ComboBox de cantidad
			HWND hComboBoxCantidad = GetDlgItem(hwnd, CB_CANTIDAD);
			int cantidadSeleccionada = SendMessageA(hComboBoxCantidad, CB_GETCURSEL, 0, 0) +1;

			// Calcular el total de la compra
			int precio = precios[eventoSeleccionado];  
			int totalSinPromocion = precio * cantidadSeleccionada;

			// Obtener opción del ComboBox de promociones
			HWND hComboBoxPromocion = GetDlgItem(hwnd, CB_PROMO);
			int promocionSeleccionada = SendMessageA(hComboBoxPromocion, CB_GETCURSEL, 0, 0);


			 if (promocionSeleccionada == 0) { // 2x1
				totalFinal = totalSinPromocion+ (totalSinPromocion * iva);
			}
			else if (promocionSeleccionada == 1) { // 10% Descuento
				totalFinal = static_cast<int>((totalSinPromocion * (1 - descuento)) + (totalSinPromocion * iva));
			}

			// Mostrar el total sin promoción 
			HWND hEditTotalSinPromocion = GetDlgItem(hwnd, TXT_SUBTOTAL);
			char bufferTotalSinPromocion[10];
			_itoa_s(totalSinPromocion, bufferTotalSinPromocion, 10);
			SetWindowTextA(hEditTotalSinPromocion, bufferTotalSinPromocion);

			// Mostrar el total final
			HWND hEditTotalFinal = GetDlgItem(hwnd, TXT_TOTAL);
			char bufferTotalFinal[10];
			_itoa_s(totalFinal, bufferTotalFinal, 10);
			SetWindowTextA(hEditTotalFinal, bufferTotalFinal);
		}

		if (LOWORD(wParam) == BTN_COMPRAR && HIWORD(wParam) == BN_CLICKED) {
			// Obtener el evento seleccionado
			HWND hComboBoxEventos = GetDlgItem(hwnd, CB_EVENTO);
			int eventoSeleccionado = SendMessageA(hComboBoxEventos, CB_GETCURSEL, 0, 0);

			compraActual.evento = opcionesEventos[eventoSeleccionado];
			compraActual.cantidad = SendMessageA(GetDlgItem(hwnd, CB_CANTIDAD), CB_GETCURSEL, 0, 0) + 1;
			compraActual.totalFinal = totalFinal;

			// Añadir la compra al historial
			historialCompras.push_back(compraActual);

			// Volver al menú
			EndDialog(hwnd, 0);
			HWND hMenu = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(MENU), hwnd, vMenu);
			ShowWindow(hMenu, SW_SHOW);
		}

		if (LOWORD(wParam) == BTN_VOLVER2 && HIWORD(wParam) == BN_CLICKED) {
			EndDialog(hwnd, 0);
			HWND hMenu = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(MENU), hwnd, vMenu);
			ShowWindow(hMenu, SW_SHOW);
		}
	} break;
	}

	return FALSE;
}

LRESULT CALLBACK vMisCompras(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_INITDIALOG: {
		// Muestra todas las compras en el ListBox
		HWND hListBoxCompras = GetDlgItem(hwnd, LIST_COMPRAS);
		for (const Compra& compra : historialCompras) {
			char buffer[256];
			sprintf_s(buffer, "%s", compra.evento);
			SendMessageA(hListBoxCompras, LB_ADDSTRING, 0, (LPARAM)buffer);
		}
	} break;

	case WM_COMMAND: {
		if (LOWORD(wParam) == BTN_VOLVER3 && HIWORD(wParam) == BN_CLICKED) {
			EndDialog(hwnd, 0);
			HWND hMenu = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(MENU), hwnd, vMenu);
			ShowWindow(hMenu, SW_SHOW);
		}
	}
	}
	return FALSE;
}