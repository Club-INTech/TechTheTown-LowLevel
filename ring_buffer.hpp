#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

template<typename T, uint16_t BUFFER_SIZE>
class ring_buffer {
private:
    T buffer[BUFFER_SIZE];
    uint16_t current_element;
public:
    typedef T value_type;

    ring_buffer() : current_element(0) {
    }

    ~ring_buffer() {
    }

    void append(T value) {
        current_element++;
        if (current_element == BUFFER_SIZE) {
            current_element = 0;
        }
        buffer[current_element] = value;
    }

    uint16_t size() {
        return BUFFER_SIZE;
    }

    T* data() {
        return buffer;
    }

    uint16_t current() {
        return current_element;
    }
};


/**
 * A changer en un tri plus optimis�
 * FAUX ! le tri � bulles est optimis� dans le cas o� une seule valeur du tableau n'est pas tri�e !
 */
template<typename T, uint16_t BUFFER_SIZE>
void bubble_sort(ring_buffer<T, BUFFER_SIZE> & buff) {
    T* buffer = buff.data();
    for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
        for (uint16_t j = 0; j < i; j++) {
            if (buffer[i] > buffer[j]) {
                T temp = buffer[i];
                buffer[i] = buffer[j];
                buffer[j] = temp;
            }
        }
    }
}

template<typename T, typename T2>
uint32_t regression_lin(T x1, T x2, T y1, T y2, T2 x) {
    uint32_t pourcentage = (uint32_t) ((x1 - x)*100) / (x1 - x2);
    return (pourcentage * y2 + (100 - pourcentage) * y1) / 100;
}

template<typename T, uint16_t BUFFER_SIZE>
T mediane(ring_buffer<T, BUFFER_SIZE> & buff) {
    bubble_sort(buff);
    return buff.data()[BUFFER_SIZE/2];
}

#endif
