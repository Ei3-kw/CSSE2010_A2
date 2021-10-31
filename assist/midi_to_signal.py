import mido
from mido import MidiFile

mid = MidiFile('Avicii_Levels.mid')

messages = [x for x in mid if x.type == 'note_on' or x.type == 'note_off']

# C
# static uint16_t freqs
# static uint16_t times

def play():
    events = []
    notes = []
    length = []
    for i, message in enumerate(messages):
        events.append(message.type == 'note_on')

        freq = round(note_to_frequency(message.note))
        notes.append(freq)

        time = int(message.time * 1000) # time in ms
        length.append(time)

    xtr = str(tuple(zip(events, notes, length))[56:]).replace('(', '{') \
        .replace(')', '}').replace('True', '1').replace('False', '0')
    print(f'static const uint16_t freq_time[{len(messages)//2}][3]{xtr};')


def note_to_frequency(m):
    return 27.5 * 2 ** ((m - 21) / 12.0)

if __name__ == "__main__":
    # print(len(messages))
    play()