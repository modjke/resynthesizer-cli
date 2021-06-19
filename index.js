const fs = require('fs')
const { spawn } = require('child_process')
const { promisify } = require('util')
const sharp = require('sharp')
const { Writable } = require('stream')


void async function main() {
    const image = 'image.jpg'

    const { data: imageBuffer, info: imageInfo } = await sharp(image)
        .removeAlpha()
        .raw()
        .toBuffer({ resolveWithObject: true })

    if (imageInfo.channels !== 3) {
        throw new Error(`Invalid number of channels`)
    }

    const mask = 'mask.png'
    const { data: maskBuffer, info: maskInfo } = await sharp(mask)
        .toColorspace('b-w')
        .raw()
        .toBuffer({ resolveWithObject: true })

    

    if (maskInfo.width !== imageInfo.width || maskInfo.height !== imageInfo.height) {
        throw new Error(`Invalid mask dimensions`)
    }

    fs.writeFileSync('image.bin', imageBuffer, 'binary')
    fs.writeFileSync('mask.bin', maskBuffer, 'binary')
    
    const outputBuffer = fs.readFileSync('output.bin')
    
    await sharp(outputBuffer, { raw: { channels: 3, width: imageInfo.width, height: imageInfo.height }})        
        .toFile('output.png')

    return


    const cli = spawn('./cli')
    cli.stdin.setDefaultEncoding('binary')
    //const output = new BufferWritable(imageInfo.width * imageInfo.height * 3)
    cli.stdout.pipe(fs.createWriteStream('output.bin', { encoding: 'binary' }))
    
    

    const write = promisify(cli.stdin.write).bind(cli.stdin)

    const buffer = Buffer.alloc(4)
    buffer.writeInt32LE(imageInfo.width)
    await write(buffer)

    buffer.byteOffset = 0
    buffer.writeInt32LE(imageInfo.height)
    await write(buffer)    

    await write(imageBuffer)
    await write(maskBuffer)

    
}()

class BufferWritable extends Writable {
    constructor(size) {
        super({ defaultEncoding: 'binary' });

        this.buffer = Buffer.alloc(size)
        
        this.offset = 0
        
    }

    _write(chunk, encoding, callback) {                
        if (this.offset + chunk.length > this.size) {
            callback(new Error(`Out of space`))
            return
        }

        chunk.copy(this.buffer, this.offset, 0)
        this.offset += chunk.length


        callback()
    }
}