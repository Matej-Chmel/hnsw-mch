from ast import literal_eval as eval
from sys import argv

def main():
	print('\n'.join(f'{key:>20}: {args[key]!s:10}' for key in args))

if __name__ == "__main__":
	try:
		with open(argv[1]) as file:
			args = eval(file.read())
	except IndexError:
		print('No argument file was passed in.')
	except OSError:
		print('Could not read the argument file.')
	main()
