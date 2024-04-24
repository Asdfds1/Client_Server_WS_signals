import numpy as np


class SignalGenerator:
    def __init__(self,
                 signal_type="sinusoidal",
                 amplitude=1,
                 frequency=2,
                 phase=0,
                 duration=1.0,
                 pulse_duration=0.25):

        self.signal_type = signal_type
        self.amplitude = amplitude
        self.frequency = frequency
        self.phase = phase
        self.duration = duration
        self.pulse_duration = pulse_duration
        self.t = None

    def generate(self):
        if self.signal_type == "sinusoidal":
            return self.generate_sinusoidal()
        elif self.signal_type == "square":
            return self.generate_square()
        else:
            raise ValueError("Unsupported signal type")

    def generate_sinusoidal(self):
        self.t = np.linspace(0, self.duration, int(1000 * self.duration))
        signal = self.amplitude * np.sin(2 * np.pi * self.frequency * self.t + self.phase)
        return signal.tolist()

    def generate_square(self):
        self.t = np.linspace(0, self.duration, int(1000 * self.duration))
        signal = self.amplitude * ((self.t % (self.pulse_duration * 2)) < self.pulse_duration).astype(float)
        return signal.tolist()

    def get_inputs(self):
        self.signal_type = self.get_input("Enter signal type (sinusoidal or square): ")
        self.amplitude = self.get_input("Enter amplitude: ", type_func=float)
        self.duration = self.get_input("Enter duration (default 1.0): ", default=1.0, type_func=float)
        self.frequency = self.get_input("Enter frequency (default 1.0): ", default=1.0, type_func=float)
        self.phase = self.get_input("Enter phase (default 0): ", default=0, type_func=float)
        self.pulse_duration = self.get_input("Enter pulse duration: ", type_func=float)


    @staticmethod
    def get_input(prompt, default=None, type_func=lambda x: x):
        user_input = input(prompt)
        return type_func(user_input) if user_input else default


