from Metaphor import *
from abc import ABC, abstractmethod
class MetaphorModel:
    """
    Example model for metaphor representation.
    You can extend this to include useful functionality.
    """

    def get_access_status(self, status: bool) -> str:
        return "Access Granted" if status else "Access Denied"



#### observer_pattern.py (Observer Pattern Implementation)
from abc import ABC, abstractmethod
#from metaphor import MetaphorModel  # Importing from your metaphor model


# Observer Interface
class Observer(ABC):
    @abstractmethod
    def update(self, status: bool):
        pass

####################################################################################
# Design patterns : factory edition 
class vehecle(ABC):
    @abstractmethod

    def wheels(self , wheels : int):
        wheels = wheels

    def body(self , body : str ): 
        body = body

    def window(self , window : int):
        window = window 

    def doors(self , door : int):
        door = door

    def window(self , window : int):
        window = window  


class car(vehecle):
    def __init__(self):
        self.wheels = 4
        self.window = 18
        self.body = 1 
        self.doors = 4


class truck(vehecle):
    def __init__(self):
        self.wheels = 10
        self.window = 6
        self.body = 1 
        self.doors = 2



class moto(vehecle):
    def __init__(self):
        self.wheels = 2
        self.window = 1
        self.body = 1 
        self.doors = 0



class tanks(vehecle):
    def __init__(self):
        self.wheels = 2
        self.window = 1
        self.body = 1 
        self.doors = 0
##############################################################
#Design patterns : Singleton 
# 
class _Singleton():
    def ___init__(self):
        pass

    _instances = {}

    def __call__(cls):
        
        if cls not in cls._instances:
            instance = super().__call__()
            cls._instances[cls] = instance
        return cls._instances[cls]
    
###########################################################

# Subject Interface
class Subject(ABC):
    def __init__(self):
        self._observers = []

    def add_observer(self, observer: Observer):
        self._observers.append(observer)

    def remove_observer(self, observer: Observer):
        self._observers.remove(observer)

    def notify_observers(self, status: bool):
        for observer in self._observers:
            observer.update(status)


# Concrete Observer Class
class AccessObserver(Observer):
    def __init__(self, name: str, metaphor_model: MetaphorModel):
        self.name = name
        self.metaphor_model = metaphor_model
        self.access_status = None  # Tracks the access status

    def update(self, status: bool):
        self.access_status = self.metaphor_model.get_access_status(status)
        print(f"Observer {self.name}: {self.access_status}")


# Concrete Subject Class
class AccessControl(Subject):
    def __init__(self, metaphor_model: MetaphorModel):
        super().__init__()
        self._status = False  # Default access status
        self.metaphor_model = metaphor_model

    @property
    def status(self):
        return self._status

    @status.setter
    def status(self, new_status: bool):
        if self._status != new_status:
            self._status = new_status
            self.notify_observers(self._status)


# Example Usage
if __name__ == "__main__":
    

    s1 = singleton()
    s2 = singleton()

    if id(s1) == id(s2):
        print("Singleton works, both variables contain the same instance.")
    else:
        print("Singleton failed, variables contain different instances.")
