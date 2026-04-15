#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kfifo.h>

#define FIFO_SIZE 128
#define MSG_TEXT_LEN 64

// Структура сообщения
struct msg_entry {
    int id;
    char text[MSG_TEXT_LEN];
};

// Определяем и выделяем память для очереди
static DEFINE_KFIFO(msg_fifo, struct msg_entry, FIFO_SIZE);

static int __init ex_queue_init(void)
{
    int i;
    struct msg_entry msg;
    unsigned int copied;

    printk(KERN_INFO "ex_queue: Модуль загружен\n");

    // Добавляем сообщения в очередь
    for (i = 0; i < 5; i++) {
        msg.id = i + 1;
        snprintf(msg.text, MSG_TEXT_LEN, "Сообщение #%d", i + 1);
        
        // kfifo_in возвращает количество успешно записанных элементов
        copied = kfifo_in(&msg_fifo, &msg, 1);
        if (copied == 1) {
            printk(KERN_INFO "ex_queue: добавлено сообщение id=%d, текст='%s'\n", msg.id, msg.text);
        }
    }

    // Обрабатываем очередь (выводим сообщения)
    printk(KERN_INFO "ex_queue: запуск процесса обработки очередей...\n");
    while (!kfifo_is_empty(&msg_fifo)) {
        if (kfifo_out(&msg_fifo, &msg, 1) == 1) {
            printk(KERN_INFO "ex_queue: удаляется сообщение id=%d, текст='%s'\n", msg.id, msg.text);
        }
    }

    return 0; // Возвращаем 0 в случае успеха
}

static void __exit ex_queue_exit(void)
{
    struct msg_entry msg;

    printk(KERN_INFO "ex_queue: проверка очереди\n");
    // Очистка очереди при выгрузке (на случай, если что-то осталось)
    while (kfifo_len(&msg_fifo) > 0) {
        if (kfifo_out(&msg_fifo, &msg, 1) == 1) {
            printk(KERN_INFO "ex_queue: удалены id=%d\n", msg.id);
            }
    }

    printk(KERN_INFO "ex_queue: Модуль выгружен\n");
}

module_init(ex_queue_init);
module_exit(ex_queue_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anton Zubin");
MODULE_DESCRIPTION("Simple message queue example");
