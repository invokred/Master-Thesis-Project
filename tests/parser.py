import json
import sys

input_filename = sys.argv[1]
output_filename = sys.argv[2]
div = 1000000
def json_to_txt():
    with open(input_filename,'r') as f:
        json_content = json.load(f)
    
    with open(output_filename,'w') as f:
        f.write('')
    with open(output_filename,'a') as f:
        f.write(str(json_content["warehouse"]["latitude"]/div) + ' '+ str(json_content["warehouse"]["longitude"]/div) + '\n')
        f.write(str(json_content["bin"]["size"]["length"]) + ' ' + str(json_content["bin"]["size"]["width"]) + ' ' + str(json_content["bin"]["size"]["height"]) + ' ' + str(json_content["bin"]["capacity"]) + '\n')
        f.write(str(json_content["riders"])+'\n')
        f.write(str(len(json_content["packages"]))+'\n')
        for package in json_content["packages"]:
            # f.write(str(package["coordinates"]["latitude"]/div) + ' ' + str(package["coordinates"]["longitude"]/div) + ' ' + str(package["size"]["length"]) + ' ' + str(package["size"]["width"]) + ' ' + str(package["size"]["height"])+ ' ' +str(package["weight"]) + ' ' + str(package["time"]) + '\n')
            f.write(str(package["coordinates"]["latitude"]/div) + ' ' + str(package["coordinates"]["longitude"]/div) + ' ' + str(package["size"]["length"]) + ' ' + str(package["size"]["width"]) + ' ' + str(package["size"]["height"])+ ' ' +str(package["weight"]) + '\n')

    return
def txt_to_json():
    return
if __name__ == "__main__":
    json_to_txt()