const fs = require('fs')
const { spawn } = require('child_process')
const { promisify } = require('util')
const sharp = require('sharp')
const { Writable, Readable } = require('stream')


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


    const cli = spawn('./cli')
    cli.stdin.setDefaultEncoding('binary')
    const outputStream = new BufferWritable(imageBuffer.length)
    outputStream.once('finish', () => {
        sharp(outputStream.buffer, { raw: { channels: 3, width: imageInfo.width, height: imageInfo.height }})        
            .toFile('output.png')
    })
    cli.stdout.pipe(outputStream)

    const metadata = Buffer.alloc(8)
    metadata.writeInt32LE(imageInfo.width, 0)
    metadata.writeInt32LE(imageInfo.height, 4)

    const write = promisify(cli.stdin.write).bind(cli.stdin)
    await write(metadata)
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