import time

def simple_timer(seconds):
    """
    A simple countdown timer.
    :param seconds: Number of seconds to count down.
    """
    try:
        seconds = int(seconds)
        for remaining in range(seconds, 0, -1):
            print(f"Time left: {remaining} seconds", end="\n")
            time.sleep(1)
        print("\nTime's up!")
    except ValueError:
        print("Please enter a valid number of seconds.")



if __name__ == "__main__":
    s = input("enter seconds: ")
    simple_timer(s)
