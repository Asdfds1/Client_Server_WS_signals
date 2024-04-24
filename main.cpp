#include <boost/beast.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include "matplotlibcpp.h"
#include <iostream>
#include <string>
#include <vector>


namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;

namespace plt = matplotlibcpp;
using tcp = net::ip::tcp;

std::vector<std::vector<float>> parse_float_array(const std::string& data) {
    std::vector<std::vector<float>> result;
    std::stringstream ss(data);
    std::string item;

    // Флаг для определения, находимся ли мы внутри подмассива
    bool inArray = false;
    std::vector<float> currentArray;

    // Перебираем каждый символ JSON
    while (std::getline(ss, item, ',')) {
        std::stringstream itemStream(item);
        char c;
        while (itemStream >> c) {
            // Проверяем на начало или конец массива
            if (c == '[') {
                if (inArray) { // Мы внутри подмассива
                    currentArray.clear();
                } else {
                    inArray = true;
                }
            } else if (c == ']') {
                if (inArray) {
                    // Подмассив закончился, добавляем его к результату
                    result.push_back(currentArray);
                    currentArray.clear();
                    inArray = false;
                }
            } else if (std::isdigit(c) || c == '.' || c == '-') {
                // Строим число (обратите внимание, это очень упрощенный парсер)
                itemStream.putback(c);
                float value;
                itemStream >> value;
                currentArray.push_back(value);
            }
        }
    }
    return result;
}

void plot_data(const std::vector<std::vector<float>>& data, int & record_time) {

    float sampling_rate = static_cast<float>(data[0].size()) / record_time;

    std::vector<float> time_stamps(data[0].size());
    for (std::size_t i = 0; i < time_stamps.size(); ++i) {
        time_stamps[i] = i / sampling_rate;
    }

    // Генерируем вектор временных меток на основе частоты2 дискретизации.

    for (int j; j < data.size(); j++){
        std::cout << j;
        matplotlibcpp::subplot(data.size(), 1, j+1);
        matplotlibcpp::plot(time_stamps, data[j]);
    }
    matplotlibcpp::show();
}

int main() {
    try {
        // Ввод частоты и времени записи.
        int record_time;
        std::cout << "Введите частоту и время записи (в секундах): ";
        std::cin >> record_time;

        // Переменные для подключения к серверу.
        std::string host = "localhost";
        std::string port = "8080";

        // Настройка контекста io и резольвер.
        net::io_context ioc;
        tcp::resolver resolver{ioc};

        // Разрешение имени хоста и подключение к серверу WebSocket.
        auto const results = resolver.resolve(host, port);
        websocket::stream<tcp::socket> ws{ioc};
        boost::asio::connect(ws.next_layer(), results.begin(), results.end());

        // Подключение как клиент WebSocket.
        ws.handshake(host, "/");

        // Создаем буфер для приема данных и вектор для хранения сигналов.
        std::vector<std::vector<float>> signals;

        // Начало отсчета времени записи.
        auto start_time = std::chrono::steady_clock::now();

        while (std::chrono::steady_clock::now() - start_time < std::chrono::seconds(record_time)) {
            beast::multi_buffer buffer;
            ws.read(buffer);

            // Получение строки из буфера.
            std::string data = beast::buffers_to_string(buffer.data());

            // Разбор и добавление данных в вектор сигналов.
            std::vector<std::vector<float>> floats = parse_float_array(data);
            if (signals.size()<floats.size()){
                signals.resize(floats.size());
            }

            for (int i = 0; i < floats.size(); i++){
                
                signals[i].insert(signals[i].end(), floats[i].begin(), floats[i].end());
            }
        }

        // Закрытие WebSocket.
        ws.close(websocket::close_code::normal);

        std::cout << "я тут";

        // Вывод сигналов на графике.
        plot_data(signals, record_time);
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}